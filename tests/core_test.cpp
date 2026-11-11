#define HOST_TEST
#include "../app/src/main/cpp/engine.cpp"
#include <cassert>
#include <iostream>
int main(){
 assert(Engine::testNonceHex(0)=="00000000");
 assert(Engine::testNonceHex(1)=="00000001");
 assert(Engine::testNonceHex(0x12ab)=="000012ab");
 assert(Engine::testNonceHex(0x0fffffff)=="0fffffff");
 assert(Engine::testNonceHex(0x10000000)=="10000000");
 assert(Engine::testNonceHex(0xffffffff)=="ffffffff");
 for(uint32_t n: {0U,1U,0x123abU,0xffffffU,0xffffffffU}){auto encoded=Engine::testNonceHex(n);assert(encoded.size()==8);assert(std::stoul(encoded,nullptr,16)==n);}

 assert(hex(sha(Bytes{}))=="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
 assert(hex(sha(Bytes{'a','b','c'}))=="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
 auto genesis=unhex("0100000000000000000000000000000000000000000000000000000000000000000000003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a29ab5f49ffff001d1dac2b7c");
 auto gh=sha2(genesis);auto display=gh;std::reverse(display.begin(),display.end());
 assert(hex(display)=="000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");assert(meets(gh,compact(0x1d00ffff)));
 assert(target(1)==compact(0x1d00ffff));assert(hex(target(2))=="000000007fff8000000000000000000000000000000000000000000000000000");
 assert(hex(target(0.5))=="00000001fffe0000000000000000000000000000000000000000000000000000");
 J p=J::array({"job",std::string(64,'0'),"010203","04",J::array(),"01000000","1d00ffff","495fab29",true});
 auto header=Engine::testHeader(p,"05","0001");auto root=sha2(unhex("01020305000104"));assert(header.size()==80);assert(std::equal(root.begin(),root.end(),header.begin()+36));assert(hex(Bytes(header.begin(),header.begin()+4))=="00000001");assert(hex(Bytes(header.begin()+68,header.begin()+76))=="29ab5f49ffff001d");
 std::cout<<"PASS fixed-width nonce regression, SHA256 vectors, genesis SHA256d/target, difficulty, header/merkle/endian\n";
}
