/* Executes and waits for multiple child processes. */

#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  wait (exec ("child-simple", (char *[]) { "child-simple", (char *) 0 }));
  wait (exec ("child-simple", (char *[]) { "child-simple", (char *) 0 }));
  wait (exec ("child-simple", (char *[]) { "child-simple", (char *) 0 }));
  wait (exec ("child-simple", (char *[]) { "child-simple", (char *) 0 }));
}
