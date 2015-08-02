/* Tests argument passing to child processes. */

#include <syscall.h>
#include "tests/main.h"

void
test_main (void) 
{
  wait (exec ("child-args", (char *[]) { "child-args", "childarg", (char *) 0 }));
}
