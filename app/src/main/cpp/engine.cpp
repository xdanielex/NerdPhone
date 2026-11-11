#include "hash.hpp"
#include "json.hpp"
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <map>
#include <set>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <sched.h>
#include <cstring>
#include <netinet/tcp.h>
#include <netinet/in.h>
#ifndef HOST_TEST
#include <jni.h>
#endif
using J=nlohmann::json;
using Clock=std::chrono::steady_clock;
struct Job {J p;std::string ex1;size_t exSize;double diff;uint64_t generation,session;};
struct Share {std::string job,ex2,time,nonce,hash;bool block;uint64_t generation,session;};
// CPU counts are logical CPUs, not a claim about physical cores or affinity pinning.
static J cpuInfo(){
 long configured=sysconf(_SC_NPROCESSORS_CONF),online=sysconf(_SC_NPROCESSORS_ONLN);
 cpu_set_t set;CPU_ZERO(&set);J ids=J::array();int allowed=0;
 bool affinityOk=sched_getaffinity(0,sizeof(set),&set)==0;
 if(affinityOk){for(int i=0;i<CPU_SETSIZE;i++)if(CPU_ISSET(i,&set)){++allowed;ids.push_back(i);}}
 if(allowed<1)allowed=std::max(1,int(online>0?online:std::thread::hardware_concurrency()));
 return J{{"configured",std::max(1L,configured)},{"online",std::max(1L,online)},{"available",allowed},{"allowedCpuIds",ids},{"affinityDetected",affinityOk}};
}
struct WorkerStats {std::atomic<uint64_t> hashes{0};uint64_t jobHashes=0,generation=0,jobs=0;};
static std::string benchmark(int requested){
 auto info=cpuInfo();int count=std::clamp(requested,1,info["available"].get<int>());
 std::vector<uint64_t> attempts(count),checksums(count);std::vector<std::thread> threads;
 std::atomic<bool> go{false},cancel{false};Clock::time_point begin,end;
 try{for(int id=0;id<count;id++)threads.emplace_back([&,id]{
   while(!go.load(std::memory_order_acquire)&&!cancel)std::this_thread::yield();if(cancel)return;
   Bytes h(80,0);h[0]=1;HeaderHasher hasher(h);uint64_t n=id,checksum=0,total=0;
   do{auto digest=hasher.hash(uint32_t(n));checksum+=digest[0];n+=count;++total;}while((total%256)!=0||Clock::now()<end);
   attempts[id]=total;checksums[id]=checksum;
  });
 }catch(...){cancel=true;for(auto&t:threads)if(t.joinable())t.join();throw;}
 begin=Clock::now();end=begin+std::chrono::seconds(3);go.store(true,std::memory_order_release);
 for(auto&t:threads)t.join();double seconds=std::chrono::duration<double>(Clock::now()-begin).count();
 J workers=J::array();uint64_t sum=0;for(int i=0;i<count;i++){sum+=attempts[i];workers.push_back(J{{"id",i+1},{"hashes",attempts[i]},{"hashrate",attempts[i]/seconds},{"checksum",checksums[i]}});}
 return J{{"threads",count},{"seconds",seconds},{"hashes",sum},{"hashrate",sum/seconds},{"workers",workers},{"cpuInfo",info},{"offline",true}}.dump();
}
class Engine {
 std::atomic<bool> running{false}; std::thread network;std::vector<std::thread> miners;
 std::vector<std::shared_ptr<WorkerStats>> workers;int workerCount=1;
 std::mutex mu;std::condition_variable cv;std::shared_ptr<const Job> current;
 std::deque<Share> outgoing;std::atomic<uint64_t> generation{0},hashes{0};
 uint64_t sent=0,accepted=0,rejected=0,unknown=0,blocks=0,dropped=0,suggestionRequests=0;
 std::string suggestionReply="Not sent";
 uint64_t jobsReceived=0,jobsAssigned=0,jobsStarted=0,jobHashes=0,workingGeneration=0;
 std::string assignedJob,workingJob,lastMessage;bool lastClean=false;double workingDifficulty=0;
 Clock::time_point lastJobAt{},lastRxAt{};
 double difficulty=0,networkDifficulty=0;std::string status="Stopped",lastBlock,lastError;std::atomic<int> fd{-1};Clock::time_point started;
 void state(std::string s){std::lock_guard<std::mutex>l(mu);status=std::move(s);}
 void invalidate(){std::lock_guard<std::mutex>l(mu);++generation;current.reset();assignedJob.clear();workingJob.clear();workingDifficulty=0;jobHashes=0;outgoing.clear();cv.notify_all();}
 // Stratum nonce: exactly four bytes, encoded as eight zero-padded hex digits.
 static std::string nonceHex(uint32_t n){std::ostringstream s;s<<std::hex<<std::setfill('0')<<std::setw(8)<<n;return s.str();}
 static Bytes header(const Job&j,const std::string&ex2){
  auto&p=j.p;Bytes cb=unhex(p[2].get<std::string>()+j.ex1+ex2+p[3].get<std::string>());auto root=sha2(cb);
  for(auto&b:p[4]){auto x=unhex(b.get<std::string>());if(x.size()!=32)throw std::runtime_error("Invalid merkle branch");Bytes cat(root.begin(),root.end());cat.insert(cat.end(),x.begin(),x.end());root=sha2(cat);}
  auto v=unhex(p[5].get<std::string>());auto prev=unhex(p[1].get<std::string>());auto tm=unhex(p[7].get<std::string>());auto bits=unhex(p[6].get<std::string>());
  if(v.size()!=4||prev.size()!=32||tm.size()!=4||bits.size()!=4)throw std::runtime_error("Invalid header");
  std::reverse(v.begin(),v.end());for(int i=0;i<32;i+=4)std::reverse(prev.begin()+i,prev.begin()+i+4);std::reverse(tm.begin(),tm.end());std::reverse(bits.begin(),bits.end());
  v.insert(v.end(),prev.begin(),prev.end());v.insert(v.end(),root.begin(),root.end());v.insert(v.end(),tm.begin(),tm.end());v.insert(v.end(),bits.begin(),bits.end());v.resize(80);return v;
 }
 void publishHashes(const std::shared_ptr<const Job>&j,uint64_t n,int id){hashes+=n;workers[id]->hashes+=n;std::lock_guard<std::mutex>l(mu);if(workingGeneration==j->generation&&generation==j->generation){jobHashes+=n;if(workers[id]->generation==j->generation)workers[id]->jobHashes+=n;}}
 void mine(int workerId,int stride){
  while(running){std::shared_ptr<const Job>j;{std::unique_lock<std::mutex>l(mu);cv.wait(l,[&]{return !running||current;});if(!running)break;j=current;}
   try{
    Bytes ex(j->exSize,0);ex.back()=1;auto st=target(j->diff);auto nt=compact(std::stoul(j->p[6].get<std::string>(),nullptr,16));
    {std::lock_guard<std::mutex>l(mu);if(generation!=j->generation)continue;if(workingGeneration!=j->generation){++jobsStarted;workingJob=j->p[0].get<std::string>();workingGeneration=j->generation;workingDifficulty=j->diff;jobHashes=0;}workers[workerId]->generation=j->generation;workers[workerId]->jobHashes=0;++workers[workerId]->jobs;}
    while(running&&generation==j->generation){std::string ex2=hex(ex);auto h=header(*j,ex2);HeaderHasher hasher(h);uint64_t count=0;
     // Fixed residue classes partition every extranonce2 without overlap.
     for(uint64_t n=workerId;n<=UINT32_MAX&&running&&generation==j->generation;n+=stride){
      auto digest=hasher.hash(uint32_t(n));++count;
      if(meets(digest,st)||meets(digest,nt)){
       bool block=meets(digest,nt);auto display=digest;std::reverse(display.begin(),display.end());
       Share s{j->p[0].get<std::string>(),ex2,j->p[7].get<std::string>(),nonceHex(n),hex(display),block,j->generation,j->session};
       std::lock_guard<std::mutex>l(mu);if(generation==j->generation){if(block){++blocks;lastBlock=s.hash;}if(outgoing.size()<4096)outgoing.push_back(s);else if(block){outgoing.pop_front();outgoing.push_front(s);++dropped;}else ++dropped;}
      }
      if(count==4096){publishHashes(j,count,workerId);count=0;}
     }publishHashes(j,count,workerId);
     bool overflow=true;for(int i=int(ex.size())-1;i>=0;i--)if(++ex[i]!=0){overflow=false;break;}
     if(overflow){std::unique_lock<std::mutex>l(mu);cv.wait(l,[&]{return !running||generation!=j->generation;});}
    }
   }catch(const std::exception&e){state(std::string("Job error: ")+e.what());std::unique_lock<std::mutex>l(mu);cv.wait(l,[&]{return !running||generation!=j->generation;});}
  }
 }
 int connectTo(const std::string&host,const std::string&port){
  addrinfo hints{},*res=nullptr;hints.ai_socktype=SOCK_STREAM;hints.ai_family=AF_UNSPEC;int rc=getaddrinfo(host.c_str(),port.c_str(),&hints,&res);if(rc)throw std::runtime_error(std::string("DNS: ")+gai_strerror(rc));
  int result=-1;std::string reason;
  for(auto a=res;a&&running;a=a->ai_next){
   char ip[NI_MAXHOST]={};getnameinfo(a->ai_addr,a->ai_addrlen,ip,sizeof(ip),nullptr,0,NI_NUMERICHOST);
   int s=socket(a->ai_family,a->ai_socktype,a->ai_protocol);if(s<0){reason+=std::string(ip)+" socket errno="+std::to_string(errno)+"; ";continue;}
   int flags=fcntl(s,F_GETFL,0);if(flags<0||fcntl(s,F_SETFL,flags|O_NONBLOCK)<0){reason+="fcntl errno="+std::to_string(errno)+"; ";close(s);continue;}
   int one=1;setsockopt(s,SOL_SOCKET,SO_KEEPALIVE,&one,sizeof(one));setsockopt(s,IPPROTO_TCP,TCP_NODELAY,&one,sizeof(one));fd=s;
   int err=0;int r=connect(s,a->ai_addr,a->ai_addrlen);if(r==0)result=s;
   else if(errno==EINPROGRESS){err=ETIMEDOUT;for(int i=0;i<100&&running;i++){pollfd p{s,POLLOUT,0};int pr=poll(&p,1,100);if(pr<0&&errno==EINTR)continue;if(pr<0){err=errno;break;}if(pr>0){socklen_t len=sizeof(err);if(getsockopt(s,SOL_SOCKET,SO_ERROR,&err,&len)<0)err=errno;if((p.revents&POLLOUT)&&!err)result=s;else if(!err)err=ECONNABORTED;break;}}}
   else err=errno;
   if(result>=0)break;reason+=std::string(ip)+" errno="+std::to_string(err)+" "+strerror(err)+"; ";fd=-1;close(s);
  }freeaddrinfo(res);if(result<0)throw std::runtime_error("TCP: "+reason);return result;
 }
 void writeLine(int s,const J&j){auto line=j.dump()+"\n";size_t pos=0;auto deadline=Clock::now()+std::chrono::seconds(10);while(pos<line.size()&&running){ssize_t n=send(s,line.data()+pos,line.size()-pos,MSG_NOSIGNAL);if(n>0)pos+=n;else if(n<0&&(errno==EAGAIN||errno==EWOULDBLOCK||errno==EINTR)){pollfd p{s,POLLOUT,0};poll(&p,1,100);if(Clock::now()>deadline)throw std::runtime_error("Send timeout");}else {int err=errno;throw std::runtime_error("send errno="+std::to_string(err)+" "+strerror(err));}}if(pos!=line.size())throw std::runtime_error("Stopped");}
 void pool(std::string host,std::string port,std::string user,std::string pass){uint64_t session=0;
  while(running){std::map<int,Share>pending;std::string phase="TCP connection";try{
   invalidate();state("Connecting to "+host);int s=connectTo(host,port);++session;int id=1,authId=-1;bool subscribed=false,authorized=false;std::string ex1;size_t exSize=0;double diff=1.0;J waiting;
   std::set<std::string> validJobs;std::set<int> suggestionIds;
   auto request=[&](int i,const char*m,J p){writeLine(s,J{{"id",i},{"method",m},{"params",p}});if(std::strcmp(m,"mining.suggest_difficulty")==0){suggestionIds.insert(i);while(suggestionIds.size()>64)suggestionIds.erase(suggestionIds.begin());std::lock_guard<std::mutex>l(mu);++suggestionRequests;suggestionReply="Sent 0.001; waiting for pool response";}};
   phase="subscribe";request(1,"mining.subscribe",J::array({"NerdMinerV2/V1.8.3"}));auto lastRx=Clock::now(),lastTx=lastRx,connected=lastRx;
   auto publish=[&](const J&p){if(!authorized)return;if(!p.is_array()||p.size()!=9||!p[8].is_boolean())throw std::runtime_error("Invalid mining.notify");bool clean=p[8].get<bool>();std::lock_guard<std::mutex>l(mu);if(clean){validJobs.clear();outgoing.clear();}validJobs.insert(p[0].get<std::string>());uint64_t g=++generation;current=std::make_shared<Job>(Job{p,ex1,exSize,diff,g,session});++jobsAssigned;assignedJob=p[0].get<std::string>();workingJob.clear();workingDifficulty=0;jobHashes=0;lastClean=clean;difficulty=diff;auto netTarget=compact(std::stoul(p[6].get<std::string>(),nullptr,16));long double value=0,base=0;auto diffOne=compact(0x1d00ffff);for(int i=0;i<32;i++){value=value*256+netTarget[i];base=base*256+diffOne[i];}networkDifficulty=double(base/value);status="Mining";cv.notify_all();};
   std::string buffer;
   while(running){
    if(!authorized&&Clock::now()-connected>std::chrono::seconds(30))throw std::runtime_error("Timeout handshake");
    Share share;bool have=false;{std::lock_guard<std::mutex>l(mu);if(authorized&&!outgoing.empty()){share=outgoing.front();outgoing.pop_front();have=true;}}
    if(have&&share.session==session&&validJobs.count(share.job)){if(pending.size()>=4096)throw std::runtime_error("Too many unanswered shares");int n=++id;pending[n]=share;request(n,"mining.submit",J::array({user,share.job,share.ex2,share.time,share.nonce}));{std::lock_guard<std::mutex>l(mu);++sent;}lastTx=Clock::now();}
    if(authorized&&Clock::now()-lastTx>std::chrono::seconds(30)){request(++id,"mining.suggest_difficulty",J::array({0.001}));lastTx=Clock::now();}
    if(Clock::now()-lastRx>std::chrono::seconds(180))throw std::runtime_error("Pool inactive");
    pollfd pfd{s,POLLIN,0};int pr=poll(&pfd,1,have?0:50);if(pr<0){if(errno==EINTR)continue;throw std::runtime_error(std::string("poll errno=")+std::to_string(errno)+" "+strerror(errno));}if(!pr)continue;if((pfd.revents&(POLLERR|POLLNVAL))&&!(pfd.revents&POLLIN)){int err=0;socklen_t len=sizeof(err);if(getsockopt(s,SOL_SOCKET,SO_ERROR,&err,&len)<0)err=errno;throw std::runtime_error("socket SO_ERROR="+std::to_string(err)+" "+strerror(err)+" poll="+std::to_string(pfd.revents));}
    char b[8192];ssize_t n=recv(s,b,sizeof(b),0);if(n<=0){if(n<0&&(errno==EAGAIN||errno==EWOULDBLOCK||errno==EINTR))continue;if(n==0)throw std::runtime_error("EOF: pool or network closed the connection");int err=errno;throw std::runtime_error("recv errno="+std::to_string(err)+" "+strerror(err));}lastRx=Clock::now();buffer.append(b,n);if(buffer.size()>1048576)throw std::runtime_error("Message too large");
    size_t end;while((end=buffer.find('\n'))!=std::string::npos){std::string line=buffer.substr(0,end);buffer.erase(0,end+1);if(line.empty())continue;J msg=J::parse(line);std::string method=msg.value("method",std::string());{std::lock_guard<std::mutex>l(mu);lastRxAt=Clock::now();lastMessage=method.empty()?"RPC response id="+msg.value("id",J()).dump():method;}
     if(method=="mining.set_difficulty"){double d=msg.at("params").at(0).get<double>();target(d);diff=d;{std::lock_guard<std::mutex>l(mu);difficulty=d;}}
     else if(method=="mining.notify"){waiting=msg.at("params");{std::lock_guard<std::mutex>l(mu);++jobsReceived;lastJobAt=Clock::now();}publish(waiting);}
     else if(method=="mining.set_extranonce"){ex1=msg.at("params").at(0).get<std::string>();unhex(ex1);exSize=msg.at("params").at(1).get<size_t>();if(exSize<1||exSize>32)throw std::runtime_error("Unsupported extranonce2_size");invalidate();validJobs.clear();waiting=nullptr;}
     else if(method=="client.reconnect")throw std::runtime_error("Pool requested reconnect; using configured host");
     else if(method.empty()&&msg.contains("id")&&msg["id"].is_number_integer()){
      int rid=msg["id"].get<int>();bool error=msg.contains("error")&&!msg["error"].is_null();
      if(rid==1&&!subscribed){if(error)throw std::runtime_error("Subscription rejected: "+msg.dump());auto r=msg.at("result");ex1=r.at(1).get<std::string>();unhex(ex1);exSize=r.at(2).get<size_t>();if(ex1.empty()||exSize<1||exSize>32)throw std::runtime_error("Invalid extranonce");subscribed=true;authId=++id;phase="authorize";request(authId,"mining.authorize",J::array({user,pass}));request(++id,"mining.suggest_difficulty",J::array({0.001}));state("Authorizing");}
      else if(rid==authId){if(error||!msg.value("result",false))throw std::runtime_error("Worker authorization failed");authorized=true;phase="receiving jobs / mining";state("Waiting for a job");if(!waiting.is_null())publish(waiting);}
      else if(suggestionIds.erase(rid)){std::lock_guard<std::mutex>l(mu);suggestionReply=error?"Pool error: "+msg["error"].dump():"RPC reply: "+msg.value("result",J()).dump()+"; only set_difficulty assigns the target";}
      else if(pending.count(rid)){std::lock_guard<std::mutex>l(mu);if(!error&&msg.value("result",false))++accepted;else{++rejected;lastError=msg.dump();}pending.erase(rid);}
     }
    }
   }
  }catch(const std::exception&e){std::lock_guard<std::mutex>l(mu);lastError=host+":"+port+" ["+phase+"] "+e.what();status=running?"Reconnecting: "+lastError:"Stopped";unknown+=pending.size();}
   invalidate();int s=fd.exchange(-1);if(s>=0)close(s);if(running){std::unique_lock<std::mutex>l(mu);cv.wait_for(l,std::chrono::seconds(3),[&]{return !running;});}
  }state("Stopped");
 }
public:
 ~Engine(){stop();join();}
 void join(){if(network.joinable())network.join();for(auto&t:miners)if(t.joinable())t.join();miners.clear();}
 void start(std::string host,std::string port,std::string user,std::string pass,int requestedWorkers=1){if(running)return;join();{std::lock_guard<std::mutex>l(mu);workerCount=std::clamp(requestedWorkers,1,cpuInfo()["available"].get<int>());workers.clear();for(int i=0;i<workerCount;i++)workers.push_back(std::make_shared<WorkerStats>());sent=accepted=rejected=unknown=blocks=dropped=suggestionRequests=0;suggestionReply="Not sent";hashes=0;difficulty=0;networkDifficulty=0;jobsReceived=jobsAssigned=jobsStarted=jobHashes=0;workingGeneration=0;assignedJob.clear();workingJob.clear();lastMessage.clear();lastClean=false;workingDifficulty=0;lastJobAt={};lastRxAt={};lastBlock.clear();lastError.clear();started=Clock::now();}running=true;try{for(int i=0;i<workerCount;i++)miners.emplace_back(&Engine::mine,this,i,workerCount);network=std::thread(&Engine::pool,this,host,port,user,pass);}catch(...){stop();join();throw;}}
 void stop(){running=false;invalidate();int s=fd.load();if(s>=0)shutdown(s,SHUT_RDWR);cv.notify_all();}
 std::string snapshot(){std::lock_guard<std::mutex>l(mu);double secs=running?std::chrono::duration<double>(Clock::now()-started).count():0;J ws=J::array();for(size_t i=0;i<workers.size();i++)ws.push_back(J{{"id",i+1},{"hashes",workers[i]->hashes.load()},{"jobHashes",current&&workers[i]->generation==generation?workers[i]->jobHashes:0},{"jobsStarted",workers[i]->jobs}});return J{{"miningThreads",workerCount},{"workers",ws},{"running",running.load()},{"status",status},{"hashes",hashes.load()},{"seconds",secs},{"jobsReceived",jobsReceived},{"jobsAssigned",jobsAssigned},{"jobsStarted",jobsStarted},{"assignedJob",assignedJob},{"workingJob",workingJob},{"jobHashes",jobHashes},{"workingDifficulty",workingDifficulty},{"cleanJobs",lastClean},{"jobAge",lastJobAt==Clock::time_point{}?-1.0:std::chrono::duration<double>(Clock::now()-lastJobAt).count()},{"rxAge",lastRxAt==Clock::time_point{}?-1.0:std::chrono::duration<double>(Clock::now()-lastRxAt).count()},{"lastMessage",lastMessage},{"userAgent","NerdMinerV2/V1.8.3"},{"suggestionRequests",suggestionRequests},{"suggestionReply",suggestionReply},{"requestedDifficulty",0.001},{"difficulty",difficulty},{"networkDifficulty",networkDifficulty},{"sent",sent},{"accepted",accepted},{"rejected",rejected},{"unknown",unknown},{"dropped",dropped},{"blocks",blocks},{"lastBlock",lastBlock},{"error",lastError}}.dump();}
#ifdef HOST_TEST
 static std::string testNonceHex(uint32_t n){return nonceHex(n);}
 static Bytes testHeader(const J&p,const std::string&e1,const std::string&e2){return header(Job{p,e1,e2.size()/2,1,0,0},e2);}
#endif
};
#ifndef HOST_TEST
static Engine engine;
static std::mutex commandMutex;static std::atomic<bool> benchmarkBusy{false};
static std::string str(JNIEnv*e,jstring s){const char*p=e->GetStringUTFChars(s,nullptr);std::string out(p);e->ReleaseStringUTFChars(s,p);return out;}
extern "C" JNIEXPORT void JNICALL Java_it_nerdphone_Native_start(JNIEnv*e,jobject,jstring h,jstring p,jstring u,jstring pw,jint threads){try{std::lock_guard<std::mutex>l(commandMutex);if(benchmarkBusy)throw std::runtime_error("Wait for the CPU test to finish");engine.start(str(e,h),str(e,p),str(e,u),str(e,pw),threads);}catch(const std::exception&x){e->ThrowNew(e->FindClass("java/lang/IllegalStateException"),x.what());}}
extern "C" JNIEXPORT jstring JNICALL Java_it_nerdphone_Native_cpuInfo(JNIEnv*e,jobject){try{return e->NewStringUTF(cpuInfo().dump().c_str());}catch(const std::exception&x){e->ThrowNew(e->FindClass("java/lang/IllegalStateException"),x.what());return nullptr;}}
extern "C" JNIEXPORT jstring JNICALL Java_it_nerdphone_Native_benchmark(JNIEnv*e,jobject,jint count){bool owns=false;try{{std::lock_guard<std::mutex>l(commandMutex);if(benchmarkBusy||J::parse(engine.snapshot())["running"].get<bool>())throw std::runtime_error("Stop mining and wait for any CPU test to finish");benchmarkBusy=true;owns=true;}auto result=benchmark(count);benchmarkBusy=false;return e->NewStringUTF(result.c_str());}catch(const std::exception&x){if(owns)benchmarkBusy=false;e->ThrowNew(e->FindClass("java/lang/IllegalStateException"),x.what());return nullptr;}}
extern "C" JNIEXPORT void JNICALL Java_it_nerdphone_Native_stop(JNIEnv*,jobject){engine.stop();}
extern "C" JNIEXPORT jstring JNICALL Java_it_nerdphone_Native_snapshot(JNIEnv*e,jobject){return e->NewStringUTF(engine.snapshot().c_str());}
#endif
