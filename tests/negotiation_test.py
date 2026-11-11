"""RPC success must not override the server's announced difficulty."""
import socket,threading,subprocess,json,tempfile
from pathlib import Path
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='nerdphone-negotiation-') as tmp:
 exe=str(Path(tmp)/'runner')
 subprocess.run(['g++','-std=c++17','-O2','-pthread',str(root/'tests/runner.cpp'),'-o',exe],check=True)
 for rpc_ok in (True,False):
  server=socket.socket();server.bind(('127.0.0.1',0));server.listen();port=server.getsockname()[1];errors=[]
  def pool():
   try:
    c,_=server.accept();c.settimeout(8);f=c.makefile('rwb',buffering=0)
    def send(m):f.write((json.dumps(m)+'\n').encode())
    while True:
     line=f.readline()
     if not line:break
     m=json.loads(line);i=m['id']
     if m['method']=='mining.subscribe':send({'id':i,'result':[[['mining.notify','mock']],'aabb',4],'error':None})
     elif m['method']=='mining.authorize':
      send({'id':i,'result':True,'error':None})
      send({'id':None,'method':'mining.set_difficulty','params':[1]})
      send({'id':None,'method':'mining.notify','params':['job-at-one','00'*32,'010203','04',[],'20000000','1d00ffff','65000000',True]})
     elif m['method']=='mining.suggest_difficulty':
      assert m['params']==[0.001]
      send({'id':i,'result':rpc_ok,'error':None if rpc_ok else [20,'unsupported suggestion',None]})
     elif m['method']=='mining.submit':send({'id':i,'result':True,'error':None})
    c.close()
   except Exception as e:errors.append(repr(e))
  t=threading.Thread(target=pool);t.start()
  p=subprocess.run([exe,str(port)],capture_output=True,text=True,timeout=15)
  t.join(timeout=5);server.close();assert not errors,errors;assert p.returncode==0,p.stderr
  s=json.loads(p.stdout)
  assert s['difficulty']==1 and s['workingDifficulty']==1,s
  assert s['requestedDifficulty']==0.001 and s['suggestionRequests']==1,s
  assert ('RPC reply: true' if rpc_ok else 'Pool error:') in s['suggestionReply'],s
  print('PASS: suggestion RPC', 'success' if rpc_ok else 'rejection', 'does not replace pool target 1 with requested 0.001')
