#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int
main (int argc, char **argv)
{
  if (argc < 3)
    {
      printf("usage: nice VALUE COMMAND [ARG [...]]\n");
      return EXIT_FAILURE;
    }

  int niceval = atoi (argv[1]);
  char *prog = argv[2];
  char **_argv = argv + 2;

  nice(niceval);

  pid_t pid = exec(prog, _argv);
  if (-1 == pid)
    {
      printf("nice: exec of %s failed\n", argv[2]);
    }
  else
    {
      wait(pid);
    }

  return EXIT_FAILURE;
}
