#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

int main()
{
    fs::path root("C:/tools");
    unsigned int count = 0;
    unsigned int max = 0xFFFF;
    if(fs::is_directory(root)){
        fs::recursive_directory_iterator recursive_iter(root);
        for(auto &entry : recursive_iter){
            if(++count < max){
                std::cout << entry.path() << std::endl;
            }
            else {
                return 1;
            }
        }
    }
}