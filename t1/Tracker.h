#pragma once
#include <string>
#include <chrono>

using namespace std::chrono;

class time_lord
{
private:
    bool m_paused = false;
    system_clock my_clock;
    time_point<system_clock> m_start;
    time_point<system_clock> m_end;
    duration<double> m_seconds;

public:
    time_lord(){
        start();
    }
    void start();
    void stop();
    void pause();
    void resume();
    
    double elapsed_seconds();
    std::string elapsed_timestamp();
};