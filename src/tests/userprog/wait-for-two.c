/* Start two processes and then wait for them in the opposite order. */

#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  pid_t child1 = exec ("child-quiet", (char *[]) { "child-quiet", (char *) 0 });
  pid_t child2 = exec ("child-quiet", (char *[]) { "child-quiet", (char *) 0 });
  msg ("wait(exec()) = %d", wait (child2));
  msg ("wait(exec()) = %d", wait (child1));
}
