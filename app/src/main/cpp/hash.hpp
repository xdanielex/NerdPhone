#pragma once
#include <array>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#include <cmath>
using Bytes=std::vector<uint8_t>;
using Hash=std::array<uint8_t,32>;
inline Bytes unhex(const std::string&s){if(s.size()%2)throw std::runtime_error("Odd-length hex"); Bytes b; auto n=[](char c)->int{if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='f')return c-'a'+10;if(c>='A'&&c<='F')return c-'A'+10;throw std::runtime_error("Invalid hex");};for(size_t i=0;i<s.size();i+=2)b.push_back((n(s[i])<<4)|n(s[i+1]));return b;}
template<class T> std::string hex(const T&b){static const char*h="0123456789abcdef";std::string s;for(auto v:b){s+=h[v>>4];s+=h[v&15];}return s;}
inline uint32_t rr(uint32_t x,int n){return(x>>n)|(x<<(32-n));}
inline Hash sha(const Bytes& input){
 static const uint32_t k[]={0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
 uint32_t h[]={0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
 Bytes b=input;uint64_t bits=b.size()*8;b.push_back(128);while(b.size()%64!=56)b.push_back(0);for(int i=7;i>=0;--i)b.push_back(bits>>(i*8));
 for(size_t p=0;p<b.size();p+=64){uint32_t w[64];for(int i=0;i<16;i++)w[i]=(uint32_t(b[p+4*i])<<24)|(uint32_t(b[p+4*i+1])<<16)|(uint32_t(b[p+4*i+2])<<8)|b[p+4*i+3];for(int i=16;i<64;i++)w[i]=w[i-16]+(rr(w[i-15],7)^rr(w[i-15],18)^(w[i-15]>>3))+w[i-7]+(rr(w[i-2],17)^rr(w[i-2],19)^(w[i-2]>>10));uint32_t a=h[0],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],z=h[7],bb=h[1];for(int i=0;i<64;i++){uint32_t t=z+(rr(e,6)^rr(e,11)^rr(e,25))+((e&f)^(~e&g))+k[i]+w[i];uint32_t u=(rr(a,2)^rr(a,13)^rr(a,22))+((a&bb)^(a&c)^(bb&c));z=g;g=f;f=e;e=d+t;d=c;c=bb;bb=a;a=t+u;}h[0]+=a;h[1]+=bb;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=z;}
 Hash out{};for(int i=0;i<32;i++)out[i]=h[i/4]>>(24-8*(i%4));return out;
}
inline Hash sha2(const Bytes&b){auto h=sha(b);return sha(Bytes(h.begin(),h.end()));}

// Specialized 80-byte Bitcoin-header SHA256d. Generic sha/sha2 remain
// independent reference implementations for coinbase, merkle and tests.
class HeaderHasher {
 using State=std::array<uint32_t,8>;
 State mid{};
 std::array<uint32_t,16> tail{};
 static State initial(){return {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};}
 static void compress(State& h,const std::array<uint32_t,16>& words){
 static const uint32_t k[]={0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
 uint32_t w[64];std::copy(words.begin(),words.end(),w);
 for(int i=16;i<64;i++)w[i]=w[i-16]+(rr(w[i-15],7)^rr(w[i-15],18)^(w[i-15]>>3))+w[i-7]+(rr(w[i-2],17)^rr(w[i-2],19)^(w[i-2]>>10));uint32_t a=h[0],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],z=h[7],bb=h[1];for(int i=0;i<64;i++){uint32_t t=z+(rr(e,6)^rr(e,11)^rr(e,25))+((e&f)^(~e&g))+k[i]+w[i];uint32_t u=(rr(a,2)^rr(a,13)^rr(a,22))+((a&bb)^(a&c)^(bb&c));z=g;g=f;f=e;e=d+t;d=c;c=bb;bb=a;a=t+u;}h[0]+=a;h[1]+=bb;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=z;
 }
 static uint32_t word(const Bytes& b,size_t p){return (uint32_t(b[p])<<24)|(uint32_t(b[p+1])<<16)|(uint32_t(b[p+2])<<8)|b[p+3];}
 public:
 explicit HeaderHasher(const Bytes& header){
  if(header.size()!=80)throw std::runtime_error("HeaderHasher requires an 80-byte header");
  mid=initial();std::array<uint32_t,16> first{};
  for(int i=0;i<16;i++)first[i]=word(header,4*i);
  compress(mid,first);
  for(int i=0;i<3;i++)tail[i]=word(header,64+4*i);
  tail[4]=0x80000000;tail[15]=640;
 }
 Hash hash(uint32_t nonce) const {
  auto block=tail;
  // Header nonce is little endian; SHA message words are big endian.
  block[3]=((nonce&0xff)<<24)|((nonce&0xff00)<<8)|((nonce>>8)&0xff00)|(nonce>>24);
  auto first=mid;compress(first,block);
  std::array<uint32_t,16> second{};
  std::copy(first.begin(),first.end(),second.begin());
  second[8]=0x80000000;second[15]=256;
  auto final=initial();compress(final,second);
  Hash out{};for(int i=0;i<32;i++)out[i]=final[i/4]>>(24-8*(i%4));return out;
 }
};

// All targets are big endian. Hash digest is interpreted as a little-endian integer.
inline bool meets(const Hash&h,const Hash&t){for(int i=0;i<32;i++){if(h[31-i]<t[i])return true;if(h[31-i]>t[i])return false;}return true;}
inline Hash compact(uint32_t bits){int e=bits>>24;uint32_t m=bits&0x7fffff;if((bits&0x800000)||!m||e>32)throw std::runtime_error("Invalid nBits");Hash t{};if(e<=3){m>>=8*(3-e);for(int i=0;i<3;i++)t[31-i]=m>>(8*i);}else{t[32-e]=m>>16;t[33-e]=m>>8;t[34-e]=m;}return t;}
// Exact integer division for the binary64 difficulty received over Stratum.
inline Hash target(double d){if(!std::isfinite(d)||d<=0)throw std::runtime_error("Invalid difficulty");int e;double f=std::frexp(d,&e);uint64_t divisor=uint64_t(std::ldexp(f,53));int shift=53-e;Bytes num(32);auto one=compact(0x1d00ffff);std::copy(one.begin(),one.end(),num.begin());if(shift>=0){for(int j=0;j<shift;j++){int carry=0;for(int i=int(num.size())-1;i>=0;i--){int v=num[i]*2+carry;num[i]=v;carry=v>>8;}if(carry)num.insert(num.begin(),carry);}}else{for(int j=0;j<-shift;j++){int carry=0;for(auto&v:num){int n=v;v=(n>>1)|carry;carry=(n&1)<<7;}}}uint64_t rem=0;for(auto&v:num){uint64_t x=(rem<<8)|v;v=x/divisor;rem=x%divisor;}while(num.size()>32&&num[0]==0)num.erase(num.begin());Hash t{};if(num.size()>32)t.fill(255);else std::copy(num.begin(),num.end(),t.end()-num.size());return t;}
