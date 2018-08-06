#include <iostream>
#include <string>

/**/
#include "posh.h"
#if defined POSH_OS_LINUX && defined POSH_COMPILER_GCC
    #include <stdio.h>
    #include <execinfo.h>
    #include <cxxabi.h>
    #include <dlfcn.h>
    #include <stdlib.h>
    #include <sstream>
    std::string stacktrace() {
        using namespace abi;

        // retrieve call-stack
        void * trace[MAX_DEPTH];
        int stack_depth = backtrace(trace, MAX_DEPTH);
        std::ostringstream os;

        for (int i = num_discard+1; i < stack_depth; i++) {
            Dl_info dlinfo;
            if(!dladdr(trace[i], &dlinfo))
                break;

            const char * symname = dlinfo.dli_sname;

            int    status;
            char * demangled = abi::__cxa_demangle(symname, NULL, 0, &status);
            if(status == 0 && demangled)
                symname = demangled;

            //printf("entry: %s, %s\n", dlinfo.dli_fname, symname);

            // store entry to stack
            if (dlinfo.dli_fname && symname) {
                entry e;
                e.file     = dlinfo.dli_fname;
                e.line     = 0; // unsupported
                e.function = symname;
                //stack.push_back(e);
                //os << e << std::endl;
                os << dlinfo.dli_fname << " (0): " << symname << "\n";
            } else {
                break; // skip last entries below main
            }

            if (demangled)
                free(demangled);
        }
        return os.str();
    }
#elif defined POSH_COMPILER_MSVC
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
#elif defined POSH_COMPILER_GCC
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
#error "build environment not supported"
#endif/**/


void display_stacktrace(){
    //std::cout << TraceStack();
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