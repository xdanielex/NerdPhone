#define HOST_TEST
#include "../app/src/main/cpp/engine.cpp"
#include <cassert>
#include <iostream>
int main(){auto c=cpuInfo();assert(c["available"].get<int>()>=1);int n=std::min(2,c["available"].get<int>());auto b=J::parse(benchmark(n));assert(b["threads"]==n);assert(b["seconds"].get<double>()>=3);assert(b["hashrate"].get<double>()>0);assert(b["offline"]==true);for(auto&w:b["workers"])assert(w["hashes"].get<uint64_t>()>0);for(int stride=1;stride<=32;stride++){std::set<uint64_t>seen;for(int id=0;id<stride;id++)for(uint64_t nonce=id;nonce<10000;nonce+=stride)assert(seen.insert(nonce).second);assert(seen.size()==10000);}std::cout<<"PASS CPU detection, offline multi-worker benchmark and nonce partitions\n"<<b.dump(2)<<std::endl;}
