/* -*- c -*- */

#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  int handle;
  pid_t child;
  char buffer[16];

  /* Open child-rox, read from it, write back same data. */
  CHECK ((handle = open ("child-rox")) > 1, "open \"child-rox\"");
  CHECK (read (handle, buffer, sizeof buffer) == (int) sizeof buffer,
         "read \"child-rox\"");
  seek (handle, 0);
  CHECK (write (handle, buffer, sizeof buffer) == (int) sizeof buffer,
         "write \"child-rox\"");

  /* Execute child-rox and wait for it. */
  CHECK ((child = exec ("child-rox", (char *[]) { "child-rox", CHILD_CNT, (char *) 0 })) != -1, "exec \"%s %s\"", "child-rox", CHILD_CNT);
  quiet = true;
  CHECK (wait (child) == 12, "wait for child");
  quiet = false;

  /* Write to child-rox again. */
  seek (handle, 0);
  CHECK (write (handle, buffer, sizeof buffer) == (int) sizeof buffer,
         "write \"child-rox\"");
}
