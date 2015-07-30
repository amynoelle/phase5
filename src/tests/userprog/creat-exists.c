/* Verifies that trying to creat a file under a name that
   already exists will fail. */

#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  CHECK (creat ("quux.dat", 0), "creat quux.dat");
  CHECK (creat ("warble.dat", 0), "creat warble.dat");
  CHECK (!creat ("quux.dat", 0), "try to re-creat quux.dat");
  CHECK (creat ("baffle.dat", 0), "creat baffle.dat");
  CHECK (!creat ("warble.dat", 0), "try to re-creat quux.dat");
}
