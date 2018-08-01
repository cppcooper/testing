#include "Tracker.h"
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <tuple>
 
template <typename Container, typename Fun>
void tuple_for_each(const Container& c, Fun fun)
{
    for (auto& e : c)
        fun(std::get<0>(e), std::get<1>(e), std::get<2>(e));
}
 
std::string formatDuration(seconds time)
{
    using T = std::tuple<seconds, int, const char *>;
    
    constexpr T formats[] = {
        T{hours(1), 2, ""},
        T{minutes(1), 2, ":"},
        T{seconds(1), 2, ":"}
    };
    
    std::ostringstream o;
    tuple_for_each(formats, [&time, &o](auto denominator, auto width, auto separator) {
        o << separator << std::setw(width) << std::setfill('0') << (time / denominator);
        time = time % denominator;
    });
    return o.str();
}

void time_lord::start(){
    //printf("\nstart\n");
    m_paused = false;
    m_start = m_end = my_clock.now();
}
void time_lord::stop(){
    //printf("\nstop\n");
    if(!m_paused){
        m_paused = true;
        m_end = my_clock.now();
        m_seconds += m_end - m_start;
    }
}

void time_lord::pause(){
    //printf("\npause\n");
    m_paused = true;
    m_end = my_clock.now();
    m_seconds += m_end - m_start;
}

void time_lord::resume(){
    //printf("\nresume\n");
    m_paused = false;
    m_start = m_end = my_clock.now();
}

double time_lord::elapsed_seconds(){
    if( !m_paused ){
        duration<double> duration = my_clock.now() - m_start;
        auto temp = duration + m_seconds;
        return temp.count();
    }
    return m_seconds.count();
}

std::string time_lord::elapsed_timestamp(){
    if( !m_paused ){
        duration<double> duration = my_clock.now() - m_start;
        auto temp = std::chrono::duration_cast<std::chrono::seconds>(duration + m_seconds);
        return formatDuration(temp);
    }
    return formatDuration(std::chrono::duration_cast<std::chrono::seconds>(m_seconds));
}
