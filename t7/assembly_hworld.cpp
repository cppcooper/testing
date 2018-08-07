// InlineAssembler_Calling_C_Functions_in_Inline_Assembly.cpp

#include <stdio.h>
// definition of constant string
char format[] = "%s %s\n";
char hello[] = "Hello";
char world[] = "earth";
int main( void )
{
__asm // start ASM code here
 {
mov  eax, offset world // move the address of world to eax
  push eax         // Push the address of world to the stack
  mov  eax, offset hello // eax = &amp;(hello[0])
  push eax               // push hello to the stack
  mov  eax, offset format // eax = format
  push eax                // push format to the stack
  call printf // here is the problem printf is not define
// at the time of compilation.
// It will be define in run time.
//clean up the stack so that main can exit cleanly
//use the unused register ebx to do the cleanup
  pop  ebx
pop  ebx
pop  ebx
} // end of ASM code
}