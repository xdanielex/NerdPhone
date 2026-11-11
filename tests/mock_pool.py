"""Offline integration: independent hashlib validation of native shares."""
import socket,threading,subprocess,json,hashlib,time,tempfile,os
from fractions import Fraction
from pathlib import Path
root=Path(__file__).resolve().parents[1]
temp=tempfile.TemporaryDirectory(prefix='nerdphone-test-')
exe=str(Path(temp.name)/'runner')
subprocess.run(['g++','-std=c++17','-O2','-pthread',str(root/'tests/runner.cpp'),'-o',exe],check=True)
server=socket.socket();server.bind(('127.0.0.1',0));server.listen();port=server.getsockname()[1]
errors=[];counts={'valid':0,'reject':0,'jobs':0}; seen=set()
exsize=int(os.environ.get('NERDPHONE_TEST_EXSIZE','8'))
prev_wire=bytes(range(32))
branches=['11'*32,'22'*32]
def double(b):return hashlib.sha256(hashlib.sha256(b).digest()).digest()
def pool():
 try:
  conn,_=server.accept();conn.settimeout(8);f=conn.makefile('rwb',buffering=0)
  def send(x):f.write((json.dumps(x)+'\n').encode())
  def job(clean=True):
   counts['jobs']+=1
   send({'id':None,'method':'mining.notify','params':[f"job{counts['jobs']}",prev_wire.hex(),'010203','04',branches,'20000000','1d00ffff','65000000',clean]})
  submitted=0
  while True:
   line=f.readline()
   if not line:break
   m=json.loads(line);method=m['method'];p=m['params'];rid=m['id']
   if method=='mining.subscribe':
    assert p==['NerdMinerV2/V1.8.3'];send({'id':rid,'result':[[['mining.notify','mock']],'aabb',exsize],'error':None})
   elif method=='mining.authorize':assert p==['test.worker','x'];send({'id':rid,'result':True,'error':None})
   elif method=='mining.suggest_difficulty':
    assert p==[0.001],p
    send({'id':rid,'result':True,'error':None});send({'id':None,'method':'mining.set_difficulty','params':[1e-6]});job()
   elif method=='mining.submit':
    assert len(p)==5 and p[0]=='test.worker' and len(p[2])==2*exsize
    assert isinstance(p[4],str) and len(p[4])==8 and all(c in '0123456789abcdef' for c in p[4]), ('No nonce: expected exactly eight hex characters',p[4])
    assert tuple(p) not in seen;seen.add(tuple(p))
    root_hash=double(bytes.fromhex('010203aabb'+p[2]+'04'))
    for branch in branches:root_hash=double(root_hash+bytes.fromhex(branch))
    prev_header=b''.join(prev_wire[i:i+4][::-1] for i in range(0,32,4))
    header=bytes.fromhex('00000020')+prev_header+root_hash+bytes.fromhex(p[3])[::-1]+bytes.fromhex('ffff001d')+int(p[4],16).to_bytes(4,'little')
    digest=double(header);target=int(Fraction(0xffff*2**208)/Fraction.from_float(1e-6))
    assert int.from_bytes(digest,'little')<=target, 'Invalid share!'
    counts['valid']+=1;submitted+=1
    reject=submitted%3==0
    counts['reject']+=reject
    send({'id':rid,'result':not reject,'error':[23,'test rejection',None] if reject else None})
    if submitted==5:job(False)
    if submitted==10:job(True)
  conn.close()
 except (BrokenPipeError, ConnectionResetError):
  # The runner stops and closes after its timed snapshot; a reply may race
  # that close. Its exit code, job state and validated-share counts are
  # still checked below, so protocol/hash assertion failures are not hidden.
  pass
 except Exception as e:errors.append(repr(e))
thread=threading.Thread(target=pool);thread.start()
requested=int(os.environ.get('NERDPHONE_TEST_THREADS','1'))
p=subprocess.run([exe,str(port),str(requested)],capture_output=True,text=True,timeout=20)
thread.join(timeout=10);server.close()
assert p.returncode==0,p.stderr
s=json.loads(p.stdout)
assert not errors,errors
assert counts['valid']>=10,counts
assert s['accepted']>0 and s['rejected']>0,s
assert counts['jobs']==3,counts
assert s['hashes']>0,s
assert s['jobsReceived']==3 and s['jobsAssigned']==3 and 1<=s['jobsStarted']<=3,s
assert s['workingJob']=='job3' and s['jobHashes']>0,s
assert s['workingDifficulty']==1e-6,s
assert s['suggestionRequests']>=1,s
assert s['suggestionReply'].startswith('RPC reply: true'),s
print('PASS mock Stratum: handshake, independent share validation, clean/non-clean jobs, accepted/rejected accounting')
print(json.dumps({'validated':counts,'snapshot':s},indent=2))

assert s["miningThreads"]>=1 and s["miningThreads"]<=requested,s
assert len(s["workers"])==s["miningThreads"],s
assert all(w["hashes"]>0 for w in s["workers"]),s
print("PASS multi-worker telemetry and independent duplicate-free share validation:",s["miningThreads"],"miners")
