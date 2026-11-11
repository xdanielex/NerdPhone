# Creates a fresh test-only public address, authorizes briefly, submits NO work.
import socket,json,time,hashlib,os
import subprocess
key=subprocess.check_output(['openssl','ecparam','-name','secp256k1','-genkey','-noout'])
der=subprocess.run(['openssl','pkey','-pubout','-outform','DER'],input=key,capture_output=True,check=True).stdout
point=der[-65:];assert point[0]==4
pub=bytes([2+(point[-1]&1)])+point[1:33]
payload=b'\x00'+hashlib.new('ripemd160',hashlib.sha256(pub).digest()).digest()
b=payload+hashlib.sha256(hashlib.sha256(payload).digest()).digest()[:4]
alphabet='123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz';i=int.from_bytes(b,'big');address=''
while i: i,rem=divmod(i,58);address=alphabet[rem]+address
address='1'*(len(b)-len(b.lstrip(b'\0')))+address
out={'pool':'pool.nerdminers.org:3333','userAgent':'NerdMinerV2/V1.8.3','requestedDifficulty':0.001,'passwordHint':os.environ.get('PROBE_PASSWORD','x'),'authorizedWithFreshTestAddress':True,'sharesSubmitted':0,'messages':[]}
try:
 with socket.create_connection(('pool.nerdminers.org',3333),timeout=8) as s:
  s.settimeout(1);s.sendall((json.dumps({'id':1,'method':'mining.subscribe','params':[out['userAgent']]})+'\n').encode())
  end=time.monotonic()+7;buf=b'';authed=False
  while time.monotonic()<end:
   try:
    data=s.recv(131072)
    if not data:out['closedByServer']=True;break
    buf+=data
    while b'\n' in buf:
     line,buf=buf.split(b'\n',1)
     if not line:continue
     m=json.loads(line)
     if m.get('method')=='mining.notify':
      out['messages'].append({'method':'mining.notify','jobId':m['params'][0],'cleanJobs':m['params'][8]})
     else:out['messages'].append(m)
     if m.get('id')==1 and not authed:
      authed=True
      for msg in [{'id':2,'method':'mining.authorize','params':[address,os.environ.get('PROBE_PASSWORD','x')]},{'id':3,'method':'mining.suggest_difficulty','params':[0.001]}]:s.sendall((json.dumps(msg)+'\n').encode())
   except socket.timeout:continue
except Exception as e:out['error']=str(e)
print(json.dumps(out,indent=2))
