#include <iostream>
#include <string>

#include "posh.h"
#if defined POSH_OS_LINUX
    //research what to include
#elif defined POSH_COMPILER_MSVC || defined POSH_COMPILER_GCC
    #include "StackWalker.h"
    class MyStackWalker : public StackWalker
    {
    private:
        std::string trace;
        int stack = 0;
    protected:
        virtual void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName){}
        virtual void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion){}
        virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr){}
        virtual void OnOutput(LPCSTR szText) {
            if(++stack > 2 ){
                trace.append(szText);
            }
        }

    public:
        MyStackWalker() : StackWalker(0) {}
        std::string GetCallStack(){
            stack = 0;
            trace = "";
            ShowCallstack();
            return trace;
        }
    };
    std::string stacktrace(){
        static MyStackWalker sw;
        return sw.GetCallStack();
    }
#else
"assertion failed: environment is not supported by this file"
#endif


std::string TraceStack(__int8 skip = 0,unsigned __int8 capture = 62);



void display_stacktrace(){
    std::cout << TraceStack();
    std::cout << stacktrace();
}

void s3() {
    display_stacktrace();
}

void s2() {
    s3();
}

void s1(){
    s2();
}

int main(){
    s1();
}