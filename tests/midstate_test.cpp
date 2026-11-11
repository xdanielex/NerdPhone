#include "../app/src/main/cpp/hash.hpp"
#include <random>
#include <iostream>
#include <chrono>
#include <cassert>
int main(){
 std::mt19937 rng(0x1234);
 for(int job=0;job<160;job++){
  Bytes h(80);for(auto& b:h)b=uint8_t(rng());HeaderHasher fast(h);
  for(int i=0;i<128;i++){
   uint32_t n=i==0?0:i==1?0xffffffff:i==2?0x000012ab:rng();
   for(int k=0;k<4;k++)h[76+k]=n>>(8*k);
   assert(fast.hash(n)==sha2(h));
  }
 }
 bool bad=false;try{HeaderHasher h(Bytes(79));}catch(const std::runtime_error&){bad=true;}assert(bad);
 Bytes h(80);h[0]=1;HeaderHasher fast(h);volatile uint64_t sum=0;
 auto run=[&](bool optimized){auto start=std::chrono::steady_clock::now();for(uint32_t n=0;n<300000;n++){Hash d;if(optimized)d=fast.hash(n);else{for(int k=0;k<4;k++)h[76+k]=n>>(8*k);d=sha2(h);}sum+=d[0];}return std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();};
 double old=run(false),now=run(true);
 std::cout<<"PASS: 20480 randomized/boundary header comparisons; invalid length rejected\nHost-only timing: reference="<<old<<"s midstate="<<now<<"s ratio="<<old/now<<" checksum="<<sum<<"\nNot an Android performance prediction.\n";
}
