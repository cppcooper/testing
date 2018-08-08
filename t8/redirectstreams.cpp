#include "../posh.h"
#include <iostream>
#include <fstream>
#include <io.h>

class dummybuf : public std::streambuf{}dummybuffer;


void test_conToggle();
void test_streamToggle();
void toggle_conOutput();
void toggle_streamOutput();

int main(){
    printf("stream toggle test\n");
    test_streamToggle();
    printf("\ncon toggle test\n");
    test_conToggle();
}


void test_stdout(){
    static int test = 0;
    fprintf(stdout,"stdout test %d\n",++test);
}
void test_stderr(){
    static int test = 0;
    fprintf(stderr,"stderr test %d\n",++test);
}
void test_cout(){
    static int test = 0;
    std::cout << "cout test " << ++test << std::endl;
}
void test_cerr(){
    static int test = 0;
    std::cerr << "cerr test " << ++test << std::endl;
}

void test_streamToggle(){
    printf("-DISABLING-\n");
    toggle_streamOutput();
    test_cerr();
    test_cout();
    test_stderr();
    test_stdout();
    toggle_streamOutput();
    printf("-ENABLED-\n");
    test_cerr();
    test_cout();
    test_stderr();
    test_stdout();
}

void test_conToggle(){
    printf("-DISABLING-\n");
    toggle_conOutput();
    test_cerr();
    test_cout();
    test_stderr();
    test_stdout();
    toggle_conOutput();
    printf("-ENABLED-\n");
    test_cerr();
    test_cout();
    test_stderr();
    test_stdout();
}

void toggle_streamOutput(){
    static auto cerr_orig = std::cerr.rdbuf();
    static auto cout_orig = std::cout.rdbuf();
    static bool redirected = false;
    if(!redirected){
        redirected = true;
        std::cerr.rdbuf(&dummybuffer);
        std::cout.rdbuf(&dummybuffer);
    }else{
        redirected = false;
        std::cerr.rdbuf(cerr_orig);
        std::cout.rdbuf(cout_orig);
    }
}

void toggle_conOutput(){
  static bool redirected = false;
#if defined POSH_OS_LINUX
  static bool do_stdout = isatty(fileno(stdout)) == 1;
  static bool do_stderr = isatty(fileno(stderr)) == 1;
#elif defined POSH_OS_WIN32
  static bool do_stdout = _isatty(_fileno(stdout)) != 0;
  static bool do_stderr = _isatty(_fileno(stderr)) != 0;
#endif
  if(!redirected){
    redirected = true;
#if defined POSH_OS_LINUX
    if(do_stdout) freopen( "/dev/null", "w", stdout );
    if(do_stderr) freopen( "/dev/null", "w", stderr );
#elif defined POSH_OS_WIN32
    if(do_stdout) freopen( "nul", "w", stdout );
    if(do_stderr) freopen( "nul", "w", stderr );
#else
    #error "OS not supported";
#endif
  }else{
    redirected = false;
#if defined POSH_OS_LINUX
    if(do_stdout) freopen("/dev/tty","a",stdout);
    if(do_stderr) freopen("/dev/tty","a",stderr);
#elif defined POSH_OS_WIN32
    if(do_stdout) freopen("CONOUT$","a",stdout);
    if(do_stderr) freopen("CONOUT$","a",stderr);
#else
    #error "OS not supported";
#endif
  }
}