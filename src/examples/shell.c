#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>

static void read_line (char line[], size_t);
static bool backspace (char **pos, char line[]);

int
main (void)
{
  printf ("Shell starting...\n");
  for (;;) 
    {
      bool background = false;
      char commands[80], *cptr = commands, *delim = NULL;

      /* Read command(s). */
      printf ("$ ");
      read_line (commands, sizeof commands);

      do
        {
          /* Tokenize next command. */
          delim = strpbrk(cptr, "\r;&");
          if (NULL != delim)
            {
              background = *delim == '&' ? true : false;
              *delim = '\0';
            }

          /* Execute command. */
          if (!strcmp (cptr, "exit"))
            break;
          else if (!memcmp (cptr, "cd ", 3))
            {
              if (!chdir (cptr + 3))
                printf ("\"%s\": chdir failed\n", cptr + 3);
            }
          else if (cptr[0] == '\0')
            {
              /* An empty command. */
            }
          else
            {
              pid_t pid = exec (cptr);
              if (pid != PID_ERROR)
                {
                  if (! background)
                    {
                      printf ("\"%s\": exit code %d\n", cptr, wait (pid));
                    }
                }
              else
                {
                  printf ("exec failed\n");
                }
            }
          cptr = delim + 1; /* Will point to invalid address if cptr == NULL,
                             * but the next line terminates the loop in this case.
                             */
        } while (NULL != delim);
  }

  printf ("Shell exiting.");
  return EXIT_SUCCESS;
}

/* Reads a line of input from the user into LINE, which has room
   for SIZE bytes.  Handles backspace and Ctrl+U in the ways
   expected by Unix users.  On return, LINE will always
   end in a new-line character and be null-terminated. */
static void
read_line (char line[], size_t size) 
{
  char *pos = line;
  for (;;)
    {
      char c;
      read (STDIN_FILENO, &c, 1);

      switch (c) 
        {
        case '\r':
          putchar ('\n');
          *pos++ = c;
          *pos = '\0';
          return;

        case '\b':
        case 0x7f:
          backspace (&pos, line);
          break;

        case ('U' - 'A') + 1:       /* Ctrl+U. */
          while (backspace (&pos, line))
            continue;
          break;

        default:
          /* Add character to line. */
          if (pos < line + size - 2)
            {
              putchar (c);
              *pos++ = c;
            }
          break;
        }
    }
}

/* If *POS is past the beginning of LINE, backs up one character
   position.  Returns true if successful, false if nothing was
   done. */
static bool
backspace (char **pos, char line[]) 
{
  if (*pos > line)
    {
      /* Back up cursor, overwrite character, back up
         again. */
      printf ("\b \b");
      (*pos)--;
      return true;
    }
  else
    return false;
}
