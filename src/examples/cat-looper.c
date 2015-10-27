#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

void
usage ()
{
  printf("usage: cat-looper count file\n");
  exit(EXIT_FAILURE);
}

int
main (int argc, char *argv[]) 
{
  bool success = true;

  if (argc != 3)
    {
      usage();
    }

  long n = atoi(argv[1]);
  
  for (int i = 0; i < n; i++) 
    {
      int fd = open (argv[2]);
      if (fd < 0) 
        {
          printf ("%s: open failed\n", argv[i]);
          success = false;
          continue;
        }
      for (;;) 
        {
          char buffer[1024];
          int bytes_read = read (fd, buffer, sizeof buffer);
          if (bytes_read == 0)
            break;
          write (STDOUT_FILENO, buffer, bytes_read);
        }
      close (fd);
    }
  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
