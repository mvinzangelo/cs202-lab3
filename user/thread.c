#include "thread.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

int thread_create(void *(start_routine)(void *), void *arg) {
  // printf("thread create-----\n");
  // create user stack of PGSIZE bytes
  char *c = sbrk(PGSIZE);
  // call clone
  int pid = clone((void *)c + PGSIZE);
  // printf("returned pid value: %d\n", pid);
  // for parent: return 0 on success and -1 on failure
  if (pid == -1) {
    // printf("error: not specified stack\n");
    return -1;
  }
  else if (pid > 0) {
    // printf("in parent\n");
    return 0;
  }
  // for child: - call start routine to start thread execution with input arg
  //            - when returns, terminate child thread by calling 'exit()'
  else {
    // printf("executing start routine\n");
    start_routine(arg);
    exit(0);
  }
  // printf("error: outside of branches\n");
  return 0;
}
void lock_init(struct lock_t *lock) {
  lock->locked = 0;
  return;
}
void lock_acquire(struct lock_t *lock) {
  while(__sync_lock_test_and_set(&lock->locked, 1) != 0)
  ;
  __sync_synchronize();
  return;
}
void lock_release(struct lock_t *lock) {
  __sync_synchronize();
  __sync_lock_release(&lock->locked);
  return;
}
