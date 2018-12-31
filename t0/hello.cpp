#include "Registry.hpp"   // Module to test
#include <exception>
#include <iostream>
#include <vector>
//#include <thread>
using namespace std;
using namespace winreg;


bool EnableEscapeCodes(){
    try{
        const string testSubKey = "Console";
        RegKey key( HKEY_CURRENT_USER, testSubKey );
        const DWORD value = 0x1;
        bool hascolour = key.HasDword("VirtualTerminalLevel");
        if(hascolour){
            hascolour = 0x0 < key.GetDwordValue("VirtualTerminalLevel");
        }
        key.SetDwordValue("VirtualTerminalLevel", value);
        return hascolour;
    }
    catch (const RegException& e)
    {
        cout << "\n*** Registry Exception: " << e.what();
        cout << "\n*** [Windows API error code = " << e.ErrorCode() << "\n\n";
        return false;
    }
    catch (const exception& e)
    {
        cout << "\n*** ERROR: " << e.what() << '\n';
        return false;
    }
}

int main(){
    if(EnableEscapeCodes()){
        std::cout << "\33[32mHello World.\n";
        char t;
        std::cin >> t;
        return 0;
    }
    return 1;
}