#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

void replace(fs::path&);

int main(int argc, const char* argv[]) {
    std::stringstream arg;
    if( argc >= 2 ){
        arg = std::stringstream(argv[1]);
    }
    if(arg.str().compare("pwd") == 0){
        std::cout << fs::current_path() << std::endl;
        return 0;
    } else if(arg.str().compare("mv") == 0){
        auto root = fs::current_path();
        replace(root);
    } else if(arg.str().compare("ls") == 0) {
        unsigned int count = 0;
        unsigned int max = 0xFF;
        auto root = fs::current_path();
        if(fs::is_directory(root)){
            fs::recursive_directory_iterator recursive_iter(root);
            for(auto &entry : recursive_iter){
                if(++count < max){
                    std::cout << entry.path() << std::endl;
                }
                else {
                    return 0;
                }
            }
        }
    }
    return 1;
}

void replace(fs::path& root){
    fs::directory_iterator iter(root);
    for(auto &entry : iter){
        auto path = entry.path();
        auto name = entry.path().filename().string();
        std::replace(name.begin(),name.end(),' ','-');
        path.replace_filename(name);
        if(entry.path().filename().string().compare(name) != 0){
            fs::rename(entry.path(),path);
        }
        if(entry.is_directory()){
            replace(path);
        }
    }
}/**/

/*int main(int argc, const char* argv[]) {
    std::stringstream arg;
    fs::path root = fs::current_path();
    if( argc >= 2 ){
        arg = std::stringstream(argv[1]);
        root = fs::path(arg.str());
    }
    replace(root);
    return 0;
}

void replace(fs::path& root){
    fs::directory_iterator iter(root);
    for(auto &entry : iter){
        auto path = entry.path();
        auto name = entry.path().filename().string();
        std::replace(name.begin(),name.end(),' ','-');
        path.replace_filename(name);
        if(entry.path().filename().string().compare(name) != 0){
            fs::rename(entry.path(),path);
        }
        if(entry.is_directory()){
            replace(path);
        }
    }
}/**/
