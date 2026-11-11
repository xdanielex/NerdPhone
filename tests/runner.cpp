#define HOST_TEST
#include "../app/src/main/cpp/engine.cpp"
#include <iostream>
int main(int argc,char**argv){Engine e;e.start("127.0.0.1",argv[1],"test.worker","x",argc>2?std::stoi(argv[2]):1);std::this_thread::sleep_for(std::chrono::seconds(4));std::cout<<e.snapshot()<<std::endl;e.stop();e.join();}
