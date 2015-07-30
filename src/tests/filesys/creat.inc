/* -*- c -*- */

#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"

static char buf[TEST_SIZE];

void
test_main (void) 
{
  const char *file_name = "blargle";
  CHECK (creat (file_name, TEST_SIZE), "creat \"%s\"", file_name);
  check_file (file_name, buf, TEST_SIZE);
}
