#include <iostream>
#include <fstream>

class dummybuf : public std::streambuf{};

int main(){
    dummybuf dummybuffer;
    auto orig = std::cout.rdbuf();
    std::cout.rdbuf(&dummybuffer);
    std::cout << "hello";
    std::cout.rdbuf(orig);
    std::cout << " world";
}