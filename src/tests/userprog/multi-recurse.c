/* Executes itself recursively to the depth indicated by the
   first command-line argument. */

#include <debug.h>
#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include "tests/lib.h"

const char *test_name = "multi-recurse";

int
main (int argc UNUSED, char *argv[]) 
{
  int n = atoi (argv[1]);

  msg ("begin %d", n);
  if (n != 0) 
    {
      char param[128];
      pid_t child_pid;
      int code;
      
      snprintf (param, sizeof param, "%d", n - 1);
      CHECK ((child_pid = exec ("multi-recurse", (char *[]) { "multi-recurse", (char *) param, (char *) 0 })) != -1, "exec(\"multi-recurse %s\")", param);

      code = wait (child_pid);
      if (code != n - 1)
        fail ("wait(exec(\"multi-recurse\")) returned %d", code);
    }
  
  msg ("end %d", n);
  return n;
}
