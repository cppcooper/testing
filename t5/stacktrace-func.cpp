/* This function, in its original form, was found in the comment section(comments no longer available-2017.10.29) of an msdn article 
The article was the CaptureStackBackTrace function
found: https://msdn.microsoft.com/en-us/library/windows/desktop/bb204633(v=vs.85).aspx
Which was in turn based on a StackOverflow answer

I do not believe this was the stackoverflow question referenced, however it does have identical code
https://stackoverflow.com/questions/590160/how-to-log-stack-frames-with-windows-x64
https://stackoverflow.com/q/590160

Well mine is based on that comment, because I have changed very little outside of formatting.
*/
#include <mutex>
#include <string>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <dbghelp.h>
#include <WinBase.h>
#include <fstream>
#pragma comment(lib, "Dbghelp.lib")
typedef USHORT( WINAPI *CaptureStackBackTraceType )( ULONG, ULONG, PVOID*, PULONG );


std::string TraceStack (__int8 skip,unsigned __int8 capture)
{
    std::stringstream os;
    // Quote from Microsoft Documentation:
    // ## Windows Server 2003 and Windows XP:  
    // ## The sum of the FramesToSkip and FramesToCapture parameters must be less than 63.
    const int kMaxCallers = 62;

    capture = capture > 62 ? 62 : capture;
    
    CaptureStackBackTraceType func = (CaptureStackBackTraceType)( GetProcAddress( LoadLibrary( "kernel32.dll" ), "RtlCaptureStackBackTrace" ) );
    if ( func == NULL )
    {
        return ""; // WOE 29.SEP.2010
    }

    SYMBOL_INFO* symbol = (SYMBOL_INFO *)calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

    HANDLE process = GetCurrentProcess();
    SymInitialize( process, NULL, TRUE );
    void* callers_stack[kMaxCallers];
    unsigned short frames = (func)( 0, kMaxCallers, callers_stack, NULL );

    frames = frames < ( capture + skip ) ? frames : ( capture + skip );
    os << std::endl << "\tThread ID: " << GetCurrentThreadId() << std::endl;
    for ( unsigned int i = skip; i < frames; i++ )
    {
        SymFromAddr( process, (DWORD64)( callers_stack[i] ), 0, symbol );
        os << "\t\tFrame: " << i - skip 
            << ",\t0x" << callers_stack[i] 
            << " _Symbol:\t0x" << (void*)symbol->Address 
            << ", " << symbol->Name 
            << std::endl;
    }

    free( symbol );
    return os.str();
}
