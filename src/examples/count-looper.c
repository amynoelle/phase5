#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

void
usage ()
{
  printf("usage: count-looper count\n");
  exit(EXIT_FAILURE);
}

int
main (int argc, char *argv[]) 
{
  if (argc != 2)
    {
      usage();
    }

  unsigned int n = atoi(argv[1]);
  
  unsigned long long count = 0;
  for (unsigned int i = 0; i < n; i++) 
    {
      for (unsigned int j = 0; j < n; j++) 
        {
          count++;
        }
    }
  printf("%s: done (count is %lld)\n", argv[0], count);
  return EXIT_SUCCESS;
}
