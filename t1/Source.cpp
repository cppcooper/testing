#include <windows.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <sstream>
#include "Tracker.h"
#include "key_wait.h"

time_lord bench;
bool loop = true;
bool paused = false;
int initial_time = 0;

void sleep(uint16_t milli_seconds){
    std::this_thread::sleep_for(std::chrono::milliseconds(milli_seconds));
}

void UpdateTime(){
    printf("\r                                              ");
    if (paused){
        printf("\rElapsed time: %s - PAUSED",bench.elapsed_timestamp().c_str());
        sleep(300);
    }
    else{
        printf("\rElapsed time: %s",bench.elapsed_timestamp().c_str());
        sleep(150);
    }
}

BOOL CtrlHandler( DWORD fdwCtrlType ) 
{ 
    std::chrono::system_clock::now();
    switch( fdwCtrlType ) 
    { 
    // Handle the CTRL-C signal. 
    case CTRL_C_EVENT:
        loop = false;
        bench.stop();
        sleep(1000);
//        printf("\n\n");
        printf("\r                                              ");
        printf("\rTime: %s",bench.elapsed_timestamp().c_str());
        printf("\n");
        exit(0);
    } 
    return FALSE;
} 

void TrackTime(){
    bench.start(initial_time);
    while( true ){
        short key = key_wait<void>(&UpdateTime);
        switch( key ){
            case ' ':
                if( paused ){
                    bench.resume();
                    paused = false;
                }
                else{
                    bench.pause();
                    paused = true;
                }
                break;
        }
        while(!loop){}
    }
}

int main( int argc, char* argv[] ){ 
    if( argc >= 2 ){
        std::stringstream converter(argv[1]);
        converter >> initial_time;
        if( converter.fail() ){
            std::cerr << "Invalid parameter. Please enter a number for the initial time.";
            return 2;
        }
    }
    if( SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE ) ) 
    { 
        printf("\nTime Tracker\n-----------");
        printf("\n -- Now use Ctrl+C to stop tracking and exit.");
        printf("\n -- Also, use Space to pause/resume tracking.\n\n");
        TrackTime();
    } 
    else 
    {
        printf("\nERROR: Could not set control handler");
        return 1;
    }
    return 0;
}

