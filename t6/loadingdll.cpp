/* This function, in its original form, was found in the comment section(comments no longer available-2017.10.29) of an msdn article 
The article was the CaptureStackBackTrace function
found: https://msdn.microsoft.com/en-us/library/windows/desktop/bb204633(v=vs.85).aspx
Which was in turn based on a StackOverflow answer

I do not believe this was the stackoverflow question referenced, however it does have identical code
https://stackoverflow.com/questions/590160/how-to-log-stack-frames-with-windows-x64
https://stackoverflow.com/q/590160

Well mine is based on that comment, because I have changed very little outside of formatting.
*/

#include "posh.h"
#if defined POSH_OS_LINUX
    #error "Linux is not supported"
#endif

#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <dbghelp.h>

#if defined POSH_COMPILER_MSVC
//#pragma comment(lib, "Dbghelp.lib")
    auto dbglib_handle = LoadLibrary("dbghelp.dll");
#elif defined POSH_COMPILER_GCC
    #if defined POSH_OS_WIN64
        auto dbglib_handle = LoadLibrary("./wine_dbghelpx64.dll");
    #else
        auto dbglib_handle = LoadLibrary("./wine_dbghelpx86.dll");
    #endif
#endif

#if defined POSH_OS_WIN32
auto kernel32_handle = LoadLibrary("kernel32.dll");
typedef USHORT(WINAPI *CaptureStackBackTraceFn)(ULONG,ULONG,PVOID*,PULONG);
typedef BOOL(WINAPI *SymInitFn)(HANDLE,PCSTR,BOOL);
typedef BOOL(WINAPI *SymFromAddrFn)(HANDLE,DWORD64,PDWORD64,PSYMBOL_INFO);
#endif

std::string stacktrace (__int8 skip = 0,unsigned __int8 capture = 62)
{
    if (kernel32_handle == NULL){
        std::cerr << "failed to load kernel32 dll.";
        return "";
    }
    if (dbglib_handle == NULL){
        std::cerr << "failed to load dbghelp dll.";
        return "";
    }
    auto bktrace = (CaptureStackBackTraceFn)( GetProcAddress( kernel32_handle, "RtlCaptureStackBackTrace" ) );
    auto SymInit = (SymInitFn)(GetProcAddress(dbglib_handle,"SymInitialize"));
    auto SymAddress = (SymFromAddrFn)(GetProcAddress(dbglib_handle,"SymFromAddr"));
    if (bktrace == NULL || SymInit == NULL || SymAddress == NULL){
        if(bktrace == NULL)
            std::cerr << "failed to load RtlCaptureStackBackTrace from dll.\n";
        if(SymInit == NULL)
            std::cerr << "failed to load SymFromAddr from dll.\n";
        if(SymAddress == NULL)
            std::cerr << "failed to load SymInitialize from dll.\n";
        return "";
    }

    // Quote from Microsoft Documentation:
    // ## Windows Server 2003 and Windows XP:  
    // ## The sum of the FramesToSkip and FramesToCapture parameters must be less than 63.
    std::stringstream os;
    const int kMaxCallers = 62;
    capture = capture > 62 ? 62 : capture;

    SYMBOL_INFO* symbol = (SYMBOL_INFO *)calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

//std::cout is spammed with warnings in x64, so we want to reroute that to a dummy buffer
#if defined POSH_OS_WIN64
    class dummybuf : public std::streambuf{};
    dummybuf dummybuffer;
    auto orig = std::cout.rdbuf();
    std::cout.rdbuf(&dummybuffer);
#endif
    HANDLE process = GetCurrentProcess();
    //SymInitialize( process, NULL, TRUE );
    SymInit( process, NULL, TRUE );
#if defined POSH_OS_WIN64
    std::cout.rdbuf(orig);
#endif

    void* callers_stack[kMaxCallers];
    unsigned short frames = bktrace( 0, kMaxCallers, callers_stack, NULL );

    frames = frames < ( capture + skip ) ? frames : ( capture + skip );
    os << std::endl << "\tThread ID: " << GetCurrentThreadId() << std::endl;
    for ( unsigned int i = skip; i < frames; i++ )
    {
        //SymFromAddr( process, (DWORD64)( callers_stack[i] ), 0, symbol );
        SymAddress( process, (DWORD64)( callers_stack[i] ), 0, symbol );
        os << "\t\tFrame: " << i - skip 
            << ",\t" << callers_stack[i] 
            << " _Symbol:\t0x" << (void*)symbol->Address 
            << ", " << symbol->Name
            << std::endl;
    }

    free( symbol );
    return os.str();
}

void f2(){
    std::cout << stacktrace();
}

void f1(){
    f2();
}

int main(){
    std::ofstream errorLog("errors.log");
    auto orig = std::cerr.rdbuf();
    std::cerr.rdbuf(errorLog.rdbuf());
    f1();
    std::cerr.rdbuf(orig);
}