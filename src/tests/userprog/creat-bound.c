/* Opens a file whose name spans the boundary between two pages.
   This is valid, so it must succeed. */

#include <syscall.h>
#include "tests/userprog/boundary.h"
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  msg ("creat(\"quux.dat\"): %d",
       creat (copy_string_across_boundary ("quux.dat"), 0));
}
