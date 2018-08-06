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
#include <sstream>
#include <iostream>
#include <windows.h>
#include <dbghelp.h>

#if defined POSH_COMPILER_MSVC
//#pragma comment(lib, "Dbghelp.lib")
auto dbglib_handle = LoadLibrary("dbghelp.dll");
#elif defined POSH_COMPILER_GCC
auto dbglib_handle = LoadLibrary("./dbghelp.dll");
#endif

#if defined POSH_OS_WIN32 || defined POSH_OS_WIN64
auto kernel32_handle = LoadLibrary("kernel32.dll");
#endif

std::string stacktrace (__int8 skip = 0,unsigned __int8 capture = 62)
{
    std::stringstream os;
    // Quote from Microsoft Documentation:
    // ## Windows Server 2003 and Windows XP:  
    // ## The sum of the FramesToSkip and FramesToCapture parameters must be less than 63.
    const int kMaxCallers = 62;

    capture = capture > 62 ? 62 : capture;
    typedef USHORT(WINAPI *CaptureStackBackTraceFn)(ULONG,ULONG,PVOID*,PULONG);
    typedef BOOL(WINAPI *SymInitFn)(HANDLE,PCSTR,BOOL);
    typedef BOOL(WINAPI *SymFromAddrFn)(HANDLE,DWORD64,PDWORD64,PSYMBOL_INFO);
    auto bktrace = (CaptureStackBackTraceFn)( GetProcAddress( kernel32_handle, "RtlCaptureStackBackTrace" ) );
    
    auto SymInit = (SymInitFn)(GetProcAddress(dbglib_handle,"SymInitialize"));
    auto SymAddress = (SymFromAddrFn)(GetProcAddress(dbglib_handle,"SymFromAddr"));

    if ( bktrace == NULL )
    {
        return ""; // WOE 29.SEP.2010
    }

    SYMBOL_INFO* symbol = (SYMBOL_INFO *)calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

    HANDLE process = GetCurrentProcess();
    //SymInitialize( process, NULL, TRUE );
    SymInit( process, NULL, TRUE );
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
            << ", " << symbol->Register
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
    f1();
}