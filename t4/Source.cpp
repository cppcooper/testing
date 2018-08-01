#include <iostream>
#include <vector>
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

class base_exception : public std::exception{
protected:
    char msg[2048];
    const char* function;
    const char* info;
    uint32_t line;
public:
    base_exception(const char* function_, uint32_t line_, const char* info_)
        : function(function_),
          line(line_),
          info(info_){}

    void copy(char* buffer){
        int r = snprintf(buffer,2048,"[%s:%d]\n%s",function,line,info);
        if( r != -1 && r < 2048 ) {
            return;
        }
        snprintf(buffer,2048,"[%s:%d]\n",function,line);
        size_t l = strlen(buffer);
        size_t i = strlen(info);
        strncpy(buffer+l,info,(2048-l)-i);
    }
    virtual const char* what() {
        copy(msg);
        return msg;
    }
};

std::ostream& operator<<(std::ostream& os, base_exception& obj){
    os << obj.what();
    return os;
}

class bad_alloc : public base_exception{
public:
    bad_alloc(const char* location_, uint32_t line_)
        : base_exception(location_, line_,"bad allocation"){}
};

void foo(){
    throw bad_alloc(__CEFUNCTION__,__LINE__);
}

const char* operator*(const std::string &obj){
    return obj.c_str();
}

int main(){
    try{
        foo();
        //throw "hello";
        return EXIT_SUCCESS;
    }
    catch( bad_alloc e ){
        std::cout << e;
    }
    catch( std::exception e ){
        std::cout << e.what();
    }
    catch( const char* error ){
        std::cout << error;
    }
    catch( ... ){
        std::cout << "Unknown exception thrown";
    }
    return EXIT_FAILURE;
}