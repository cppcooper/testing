#include <vector>
#include <iostream>
#include <mutex>
#include <string>
#include <cstring>

#ifdef _MSC_VER
    #define __CEFUNCTION__ __FUNCTION__
    #define __STRCPY__ strcpy_s
    inline std::string ExtractClassName(const char* func){
        std::string raw(func);
        std::string classname;
        if(raw.find("lambda") != std::string::npos){
            classname = raw.substr(0,raw.rfind("::TypeName"));
        } else {
            classname = raw.substr(0,raw.rfind("::"));
        }
        if(classname.find("<") != std::string::npos){
            size_t pos = classname.find("class ");
            do{
                classname.erase(pos,6);
                pos = classname.find("class ");
            } while( pos != std::string::npos );
            pos = classname.find(" ");
            if (pos == std::string::npos ){
                return classname;
            }
            do{
                classname.erase(pos,1);
                pos = classname.find(" ");
            } while( pos != std::string::npos );
        }
        return classname;
    }
#else
    #define __CEFUNCTION__ __PRETTY_FUNCTION__
    #define __STRCPY__ strcpy
    inline std::string ExtractClassName(const char* func){
        std::string raw(func);
        std::string classname;
        if(raw.find("lambda") != std::string::npos){
            classname = raw.substr(0,raw.rfind("::", raw.find("(")));
        } else {
            classname = raw.substr(raw.find(" ",raw.find(" ")+1)+1,raw.rfind("::",raw.find("(")));
        }
        if(classname.find("<") != std::string::npos){
            std::string templatetypeinfo;
            size_t p1 = raw.find("=")+2;
            size_t p2 = raw.find("]");
            templatetypeinfo = raw.substr(p1,p2-p1);
            classname = classname.substr(0,classname.find("<")) + "<" + templatetypeinfo + ">";
        } else {
            classname = classname.substr(0,classname.rfind("::"));
        }
        return classname;
    }
#endif



#define TYPENAMEAVAILABLE \
public: \
virtual const char* TypeName() { \
    static char name[256] = {}; \
    static std::once_flag flag; \
    std::call_once(flag, [&](){ \
        __STRCPY__(name,ExtractClassName(__CEFUNCTION__).c_str()); \
    });\
    return name; \
} \
private: 

namespace foobar{
class foo{
    TYPENAMEAVAILABLE

    public:
    static void bar(){
        std::cout << ExtractClassName(__CEFUNCTION__);
    }
};
}

template<typename QT>
class A{
  TYPENAMEAVAILABLE
public:
    static void bar(){
        std::cout << ExtractClassName(__CEFUNCTION__);
    }
};

int main() {
    A<foobar::foo> a;
    foobar::foo b;

  std::cout << a.TypeName() << std::endl;
  std::cout << b.TypeName() << std::endl;
  foobar::foo::bar();
  std::cout << std::endl;
  A<std::vector<foobar::foo>>::bar();
}