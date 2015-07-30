/* Passes a bad pointer to the creat system call,
   which must cause the process to be terminated with exit code
   -1. */

#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  msg ("creat(0x20101234): %d", creat ((char *) 0x20101234, 0));
}
