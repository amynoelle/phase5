#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

/* Maximum number of open files per thread. */
#define MAX_FILES 128
#define MAX_CHILD 128

typedef int pid_t;
#define PID_ERROR ((pid_t) -1)          /* Error value for pid_t. */

struct pcb
  {
    struct thread *thread;              /* Kernel thread associated with process. */
    pid_t pid;                          /* The process ID. */
    int exit_code;                      /* This process's completion status. */
    struct semaphore dead;              /* 1=I am alive, 0=I am dead. */
    struct pcb *children[MAX_CHILD];    /* This process' children. */
    struct file *bin_file;              /* Executable. */
    struct file *fds[MAX_FILES];        /* Array of open file descriptors. */
  };

void process_init (void);
void init_process (struct pcb *pcb, struct thread *thread);
pid_t process_execute (const char *path, char **argv);
int process_wait (pid_t);
void process_exit (void);
void process_notify_parent (void);
void process_activate (void);

#endif /* userprog/process.h */
