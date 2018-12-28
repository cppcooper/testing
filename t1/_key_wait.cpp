#include "key_wait.h"

short key_wait()
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
        while(!_kbhit()){}
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