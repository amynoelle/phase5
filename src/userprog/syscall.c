#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include <kernel/hash.h>
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/interrupt.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static int sys_halt (void);
static int sys_exit (int status);
static int sys_exec (const char *ufile);
static int sys_wait (tid_t);
static int sys_creat (const char *upath, unsigned initial_size);
static int sys_unlink (const char *upath);
static int sys_open (const char *upath);
static int sys_filesize (int fd);
static int sys_read (int fd, void *udst_, unsigned size);
static int sys_write (int fd, void *usrc_, unsigned size);
static int sys_seek (int fd, unsigned position);
static int sys_tell (int fd);
static int sys_close (int fd);
static int sys_semcreat (const char *name, int initial_value);
static int sys_semdestroy (const char *name);
static int sys_semwait (const char *name);
static int sys_semsignal (const char *name);
 
static void syscall_handler (struct intr_frame *);
static void copy_in (void *, const void *, size_t);
 
/* Serializes file system operations. */
static struct lock fs_lock;

/* Supports semaphore operations. */
static struct lock sem_lock;
static struct hash semaphore_map;

#define SEMAPHORE_NAME_SIZE 16

struct semaphore_map_elem
  {
    struct hash_elem hash_elem;
    char name[SEMAPHORE_NAME_SIZE];
    struct semaphore semaphore;
  };

static unsigned
semname_hash (const struct hash_elem *element, void *aux UNUSED)
{
  const char *name = hash_entry (element, struct semaphore_map_elem, hash_elem)->name;
  return hash_string (name);
}

static bool
semname_less (const struct hash_elem *a, const struct hash_elem *b, void *aux UNUSED)
{
  const char *name_a = hash_entry (a, struct semaphore_map_elem, hash_elem)->name;
  const char *name_b = hash_entry (b, struct semaphore_map_elem, hash_elem)->name;
  return strcmp(name_a, name_b) < 0;
}

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init (&fs_lock);
  lock_init (&sem_lock);
  hash_init (&semaphore_map, semname_hash, semname_less, NULL);
}
 
/* System call handler. */
static void
syscall_handler (struct intr_frame *f) 
{
  typedef int syscall_function (int, int, int);

  /* A system call. */
  struct syscall 
    {
      size_t arg_cnt;           /* Number of arguments. */
      syscall_function *func;   /* Implementation. */
    };

  /* Table of system calls. */
  static const struct syscall syscall_table[] =
    {
      {0, (syscall_function *) sys_halt},
      {1, (syscall_function *) sys_exit},
      {1, (syscall_function *) sys_exec},
      {1, (syscall_function *) sys_wait},
      {2, (syscall_function *) sys_creat},
      {1, (syscall_function *) sys_unlink},
      {1, (syscall_function *) sys_open},
      {1, (syscall_function *) sys_filesize},
      {3, (syscall_function *) sys_read},
      {3, (syscall_function *) sys_write},
      {2, (syscall_function *) sys_seek},
      {1, (syscall_function *) sys_tell},
      {1, (syscall_function *) sys_close},
      {2, (syscall_function *) sys_semcreat},
      {1, (syscall_function *) sys_semdestroy},
      {1, (syscall_function *) sys_semwait},
      {1, (syscall_function *) sys_semsignal},
    };

  const struct syscall *sc;
  unsigned call_nr;
  int args[3];

  /* Get the system call. */
  copy_in (&call_nr, f->esp, sizeof call_nr);
  if (call_nr >= sizeof syscall_table / sizeof *syscall_table)
    thread_exit ();
  sc = syscall_table + call_nr;

  /* Get the system call arguments. */
  ASSERT (sc->arg_cnt <= sizeof args / sizeof *args);
  memset (args, 0, sizeof args);
  copy_in (args, (uint32_t *) f->esp + 1, sizeof *args * sc->arg_cnt);

  /* Execute the system call,
     and set the return value. */
  f->eax = sc->func (args[0], args[1], args[2]);
}
 
/* Returns true if UADDR is a valid, mapped user address,
   false otherwise. */
static bool
verify_user (const void *uaddr) 
{
  return (uaddr < PHYS_BASE
          && pagedir_get_page (thread_current ()->pagedir, uaddr) != NULL);
}
 
/* Returns true if UADDR--UADDR + SIZE are valid, mapped user addresses,
   false otherwise. */
static bool
verify_user_range (const void *uaddr, size_t size)
{
  bool ok    = true;
  size_t offset = 0;

  while (true) {
    ok = verify_user (uaddr + offset);
    if (! ok)
      {
        goto done;
      }

    /* More to check because remaining size > page size. */
    if (size >= PGSIZE)
      {
        offset += PGSIZE;
        size   -= PGSIZE;
      }
    /* One more check because remaining size < page size but spans page boundary. */
    else if (size > PGSIZE - pg_ofs (uaddr))
      {
        offset += PGSIZE;
        size    = 0;
      }
    else
      {
        break;
      }
  }

done:
  return ok;
}

/* Copies a byte from user address USRC to kernel address DST.
   USRC must be below PHYS_BASE.
   Returns true if successful, false if a segfault occurred. */
static inline bool
get_user (uint8_t *dst, const uint8_t *usrc)
{
  int eax;
  asm ("movl $1f, %%eax; movb %2, %%al; movb %%al, %0; 1:"
       : "=m" (*dst), "=&a" (eax) : "m" (*usrc));
  return eax != 0;
}
 
/* Writes BYTE to user address UDST.
   UDST must be below PHYS_BASE.
   Returns true if successful, false if a segfault occurred. */
static inline bool
put_user (uint8_t *udst, uint8_t byte)
{
  int eax;
  asm ("movl $1f, %%eax; movb %b2, %0; 1:"
       : "=m" (*udst), "=&a" (eax) : "q" (byte));
  return eax != 0;
}
 
/* Copies SIZE bytes from user address USRC to kernel address
   DST.
   Call thread_exit() if any of the user accesses are invalid. */
static void
copy_in (void *dst_, const void *usrc_, size_t size) 
{
  uint8_t *dst = dst_;
  const uint8_t *usrc = usrc_;
 
  for (; size > 0; size--, dst++, usrc++) 
    if (usrc >= (uint8_t *) PHYS_BASE || !get_user (dst, usrc)) 
      thread_exit ();
}
 
/* Creates a copy of user string US in kernel memory
   and returns it as a page that must be freed with
   palloc_free_page().
   Truncates the string at PGSIZE bytes in size.
   Call thread_exit() if any of the user accesses are invalid. */
static char *
copy_in_string (const char *us) 
{
  char *ks;
  size_t length;
 
  ks = palloc_get_page (0);
  if (ks == NULL) 
    thread_exit ();
 
  for (length = 0; length < PGSIZE; length++)
    {
      if (us >= (char *) PHYS_BASE || !get_user ((uint8_t *) ks + length, (uint8_t *) us++)) 
        {
          palloc_free_page (ks);
          thread_exit (); 
        }
       
      if (ks[length] == '\0')
        return ks;
    }
  ks[PGSIZE - 1] = '\0';
  return ks;
}
 
/* Halt system call. */
static int
sys_halt (void)
{
  shutdown_power_off ();
}
 
/* Exit system call. */
static int
sys_exit (int exit_code) 
{
  thread_current ()->exit_code = exit_code;
  thread_exit ();
  NOT_REACHED ();
}
 
/* Exec system call. */
static int
sys_exec (const char *ufile) 
{
  tid_t tid;
  char *kfile = copy_in_string (ufile);
 
  lock_acquire (&fs_lock);
  tid = process_execute (kfile);
  lock_release (&fs_lock);
 
  palloc_free_page (kfile);
 
  return tid;
}
 
/* Wait system call. */
static int
sys_wait (tid_t child) 
{
  return process_wait (child);
}
 
/* Create system call. */
static int
sys_creat (const char *upath, unsigned initial_size) 
{
  char *kpath = copy_in_string (upath);
  bool ok;
   
  lock_acquire (&fs_lock);
  ok = filesys_create (kpath, initial_size);
  lock_release (&fs_lock);
 
  palloc_free_page (kpath);
 
  return ok;
}
 
/* Unlink system call. */
static int
sys_unlink (const char *upath) 
{
  char *kpath = copy_in_string (upath);
  bool ok;
   
  lock_acquire (&fs_lock);
  ok = filesys_remove (kpath);
  lock_release (&fs_lock);
 
  palloc_free_page (kpath);
 
  return ok;
}
 
/* Returns the next unused file descriptor or -1 if fds is full. */
static int
next_fd (void) 
{
  int fd = MAX_FILES;

  struct thread *cur = thread_current ();

  /* Skip 0 and 1 which represent stdin and stdout. */
  for (fd = STDOUT_FILENO + 1; fd < MAX_FILES; fd++)
    {
      if (NULL == cur->fds[fd])
        {
          break;
        }
    }
 
  return fd < MAX_FILES ? fd : -1;
}

/* Return file * corresponding to fd or NULL if fd is invalid. */
static struct file *
lookup_fd (int fd)
{
  struct file *f = NULL;

  struct thread *cur = thread_current ();

  if (fd < 0 || fd >= MAX_FILES)
    {
      goto done;
    }

  f = cur->fds[fd];

done:
  return f;
}

static void
close_fd (int fd)
{
  struct thread *cur = thread_current ();

  struct file *f = cur->fds[fd];
  if (NULL == f)
    {
      goto done;
    }

  lock_acquire (&fs_lock);
  file_close (f);
  lock_release (&fs_lock);

  cur->fds[fd] = NULL;

done:
  return;
}

/* Open system call. */
static int
sys_open (const char *upath)
{
  char *kpath = copy_in_string (upath);

  int fd = next_fd();
  if (-1 == fd)
    {
      goto done;
    }
 
  lock_acquire (&fs_lock);

  struct thread *cur = thread_current ();

  cur->fds[fd] = filesys_open (kpath);
  if (NULL == cur->fds[fd])
    {
      fd = -1;
    }

  lock_release (&fs_lock);

  palloc_free_page (kpath);

done:
  return fd;
}
 
/* Filesize system call. */
static int
sys_filesize (int fd) 
{
  int size = -1;

  struct file *f = lookup_fd(fd);
  if (NULL == f)
    {
      goto done;
    }
 
  lock_acquire (&fs_lock);
  size = file_length (f);
  lock_release (&fs_lock);
 
done:
  return size;
}
 
/* Read system call. */
static int
sys_read (int fd, void *udst_, unsigned size) 
{
  uint8_t *udst = udst_;
  int bytes_read = -1;

  /* We might want to wait until we actually try to store into an unmapped
   * page before terminating the process (i.e., read into one page at a time).
   * It is possible that the process would have gotten lucky and encountered
   * a short read. But the code here is simpler for students to reason about,
   * and we have not yet covered the details of paging in class.
   */
  if (! verify_user_range (udst, size))
    {
      thread_exit ();
    }

  /* Handle keyboard reads. */
  if (fd == STDIN_FILENO) 
    {
      for (bytes_read = 0; (size_t) bytes_read < size; bytes_read++)
        if (udst >= (uint8_t *) PHYS_BASE || !put_user (udst++, input_getc ()))
          {
            thread_exit ();
          }
    }
  else
    {
      /* Handle all other reads. */
      struct file *f = lookup_fd (fd);
      if (NULL == f)
        {
          goto done;
        }

      lock_acquire (&fs_lock);

      /* Read from file into memory. */
      bytes_read = file_read (f, udst, size);

      lock_release (&fs_lock);
    }
   
done:
  return bytes_read;
}
 
/* Write system call. */
static int
sys_write (int fd, void *usrc_, unsigned size) 
{
  uint8_t *usrc = usrc_;
  int bytes_written = -1;
  struct file *f = NULL;

  /* We might want to wait until we actually try to load from an unmapped
   * page before terminating the process (i.e., write one page at a time).
   * It is possible that the process would have gotten lucky and encountered
   * a short write. But the code here is simpler for students to reason about,
   * and we have not yet covered the details of paging in class.
   */
  if (! verify_user_range (usrc, size))
    {
      thread_exit ();
    }

  /* Do the write. */
  if (fd == STDOUT_FILENO)
    {
      putbuf (usrc, size);
      bytes_written = size;
    }
  else
    {
      f = lookup_fd (fd);
      if (NULL == f)
        {
          goto done;
        }

      lock_acquire (&fs_lock);
      bytes_written = file_write (f, usrc, size);
      lock_release (&fs_lock);
    }
 
done:
  return bytes_written;
}
 
/* Seek system call. */
static int
sys_seek (int fd, unsigned position) 
{
  int fnval = -1;

  struct file *f = lookup_fd (fd);
  if (NULL == f)
    {
      goto done;
    }
   
  lock_acquire (&fs_lock);
  if ((off_t) position >= 0)
    {
      file_seek (f, position);
    }
  lock_release (&fs_lock);

  fnval = 0;
 
done:
  return fnval;
}
 
/* Tell system call. */
static int
sys_tell (int fd) 
{
  int position = -1;

  struct file *f = lookup_fd (fd);
  if (NULL == f)
    {
      goto done;
    }
   
  lock_acquire (&fs_lock);
  position = file_tell (f);
  lock_release (&fs_lock);
 
done:
  return position;
}
 
/* Close system call. */
static int
sys_close (int fd)
{
  int fnval = -1;

  struct file *f = lookup_fd (fd);
  if (NULL == f)
    {
      goto done;
    }

  close_fd(fd);

done:
  return fnval;
}
 
/* On thread exit, close all open files. */
void
syscall_exit (void) 
{
  for (int fd = 0; fd < MAX_FILES; fd++)
    {
      close_fd(fd);
    }
}

/* System call which creates a semaphore. */
static int
sys_semcreat (const char *uname, int initial_value)
{
  int fnval = -1;
  struct hash_elem *elem = NULL;
  struct semaphore_map_elem *s = NULL;
  char *kname = copy_in_string (uname);

  s = palloc_get_page (0);
  if (NULL == s)
    {
      goto done;
    }

  strlcpy (s->name, kname, SEMAPHORE_NAME_SIZE);
  sema_init (&s->semaphore, initial_value);

  lock_acquire (&sem_lock);
  elem = hash_insert (&semaphore_map, &s->hash_elem);
  lock_release (&sem_lock);
  if (NULL != elem)
    {
      goto done;
    }

  fnval = 0;

done:
  if (fnval < 0)
    {
      if (NULL != s)
        {
          palloc_free_page (s);
        }
    }

  palloc_free_page (kname);

  return fnval;
}

/* System call which destroys a semaphore. */
static int
sys_semdestroy (const char *uname)
{
  int fnval = -1;
  char *kname = copy_in_string (uname);
  struct semaphore_map_elem s;

  strlcpy (s.name, kname, SEMAPHORE_NAME_SIZE);

  lock_acquire (&sem_lock);
  struct hash_elem *hash_elem = hash_find (&semaphore_map, &s.hash_elem);
  if (NULL == hash_elem)
    {
      goto done;
    }

  struct semaphore_map_elem *e = hash_entry (hash_elem, struct semaphore_map_elem, hash_elem);
  if (! list_empty (&e->semaphore.waiters))
    {
      goto done;
    }

  if (NULL == hash_delete (&semaphore_map, &e->hash_elem))
    {
      goto done;
    }

  palloc_free_page (e);

  fnval = 0;

done:
  lock_release (&sem_lock);
  palloc_free_page (kname);

  return fnval;
}

/* System call which waits on a semaphore. */
static int
sys_semwait (const char *uname)
{
  int fnval = -1;
  enum intr_level old_level;
  char *kname = copy_in_string (uname);
  struct semaphore_map_elem s;
  struct semaphore_map_elem *e;

  strlcpy (s.name, kname, SEMAPHORE_NAME_SIZE);

  lock_acquire (&sem_lock);
  struct hash_elem *hash_elem = hash_find (&semaphore_map, &s.hash_elem);
  if (NULL == hash_elem)
    {
      lock_release (&sem_lock);
      goto done;
    }

  /* Must release lock before calling sema_wait, but still do not want
     semaphore to change between hash_entry and sema_wait.
     Disable interrupts.*/
  old_level = intr_disable ();
  e = hash_entry (hash_elem, struct semaphore_map_elem, hash_elem);
  lock_release (&sem_lock);
  sema_wait(&e->semaphore);
  intr_set_level (old_level);

  fnval = 0;

done:
  palloc_free_page (kname);

  return fnval;
}

/* System call which signals a semaphore. */
static int
sys_semsignal (const char *uname)
{
  int fnval = -1;
  char *kname = copy_in_string (uname);
  struct semaphore_map_elem s;

  strlcpy (s.name, kname, SEMAPHORE_NAME_SIZE);

  lock_acquire (&sem_lock);
  struct hash_elem *hash_elem = hash_find (&semaphore_map, &s.hash_elem);
  if (NULL == hash_elem)
    {
      goto done;
    }

  struct semaphore_map_elem *e = hash_entry (hash_elem, struct semaphore_map_elem, hash_elem);
  sema_signal(&e->semaphore);

  fnval = 0;

done:
  lock_release (&sem_lock);
  palloc_free_page (kname);

  return fnval;
}
