#include <stdio.h>
#include <syscall.h>

int
main (int argc, char **argv)
{
  const char sem_name[] = "test-sem-name";
  const char sem_bad_name[] = "bad-sem-name";

  printf("%s: preparing to creat semaphore %s.\n", argv[0], sem_name);
  if(semcreat(sem_name, 0) < 0)
    {
      printf("%s: FAIL: creat failed.\n", argv[0]);
      return EXIT_FAILURE;
    }

  printf("%s: creat semaphore with same name again (should fail).\n", argv[0]);
  if(semcreat(sem_name, 0) == 0)
    {
      printf("%s: FAIL: creat did not return expected value for failure.\n", argv[0]);
      return EXIT_FAILURE;
    }

  printf("%s: preparing to wait on semaphore %s (should fail).\n", argv[0], sem_bad_name);
  if(semwait(sem_bad_name) == 0)
    {
      printf("%s: FAIL: open did not return expected value for failure.\n", argv[0]);
      return EXIT_FAILURE;
    }

  printf("%s: preparing to wait on semaphore %s.\n", argv[0], sem_name);
  if(semwait(sem_name) != 0)
    {
      printf("%s: FAIL: wait did not return expected value.\n", argv[0]);
      return EXIT_FAILURE;
    }

  printf("%s: signal complete.\n", argv[0]);

  printf("%s: preparing to destroy semaphore %s.\n", argv[0], sem_name);
  if(semdestroy(sem_name) != 0)
    {
      printf("%s: FAIL: destroy did not return expected value.\n", argv[0]);
      return EXIT_FAILURE;
    }

  printf("%s: destroy complete; TEST PASSED.\n", argv[0]);

  return EXIT_SUCCESS;
}
