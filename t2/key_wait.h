#pragma once
#include <conio.h>
#include <utility>
#include <cstdint>

short key_wait();

template<typename T, typename... Args>
short key_wait(T (*fn)(Args...), Args... args)
{
    union Key
    {
        short key;
        char  key_bytes[2];
    };
    Key input;
    input.key = 0;
    for(int i = 0; i < 2; ++i)
    {
        uint8_t j = 0;
        while(!_kbhit()){
            fn(std::forward<Args>(args)...);
        }
        input.key_bytes[i] = _getch();
        switch(input.key_bytes[i])
        {
            case 0:
            case -32:
                break;
            default:
                return input.key;
        }
    }
    return -1;
}