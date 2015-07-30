/* Tries to creat a file with the null pointer as its name.
   The process must be terminated with exit code -1. */

#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  msg ("creat(NULL): %d", creat (NULL, 0));
}
