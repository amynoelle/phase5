#include <stdio.h>
#include <syscall.h>

int
main (int argc, char **argv)
{
  const char sem_name[] = "test-sem-name";

  printf("%s: destroy semaphore %s (should fail)\n", argv[0], sem_name);
  if(semdestroy(sem_name) == 0)
    {
      printf("%s: FAIL: should not be able to destroy a semaphore upon which another process is waiting\n", argv[0]);
      return EXIT_FAILURE;
    }

  printf("%s: preparing to signal.\n", argv[0]);
  if(semsignal(sem_name) < 0)
    {
      printf("%s: FAIL: signal failed.\n", argv[0]);
      return EXIT_FAILURE;
    }

  printf("%s: signal complete.\n", argv[0]);

  return EXIT_SUCCESS;
}
