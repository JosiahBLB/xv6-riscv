#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/spinlock.h"
#include "kernel/semaphore.h"
#include "kernel/defs.h"
#include "kernel/param.h"
#include "kernel/proc.h"

void initsema(struct semaphore *s, int count) {
    s->value = count;
    initlock(&s->lk, "Counting Semaphore");
}

int downsema(struct semaphore *s) {
    acquire(&s->lk);
    while (s->value <= 0)
        sleep(s, &s->lk);
    s->value--;
    release(&s->lk);
    return s->value;
}

int upsema(struct semaphore *s) {
    acquire(&s->lk);
    s->value++;
    wakeup(s);
    release(&s->lk);
    return s->value;
}

// acquire the lock atomically
void semaacquire(struct rwsemaphore *rws) {
    push_off(); // disable interrupts
    while (__sync_lock_test_and_set(&rws->lk, 1) != 0) // acquire lock
        ;
    __sync_synchronize(); // stall processor to flush entries
}

void semarelease(struct rwsemaphore *rws) {
    __sync_synchronize(); // stall processor to flush entries
    __sync_lock_release(&rws->lk); // release lock
    pop_off(); // enable interrupts
}

void semasleep(struct rwsemaphore *rws) {
  struct proc *p = myproc();
  acquire(&p->lock); // acquire lock from current process
  semarelease(rws); // release the lock before sleeping

  // Go to sleep.
  p->chan = rws;
  p->state = SLEEPING;

  sched(); // call the scheduler

  // on re-entry
  p->chan = 0; // tidy up.
  release(&p->lock);
  semaacquire(rws); // Reacquire original lock.
}

// initialise the semaphore
void initrwsema(struct rwsemaphore *rws) {
    rws->readers = 1;
    rws->writers = 1;
    rws->lk = 0;
}

// A Reader enters room
int downreadsema(struct rwsemaphore *rws) {
    semaacquire(rws); // acquire the lock
    while (rws->writers <= 0) // wait till there are no writers
        semasleep(rws); 
    rws->readers--; // enter
    semarelease(rws); // release lock
    return rws->readers;
}

// A Reader exits room
int upreadsema(struct rwsemaphore *rws) {
    semaacquire(rws); // acquire the lock
    rws->readers++; // exit
    wakeup(rws); // signal to sleeping threads
    semarelease(rws); // release the lock
    return rws->readers;
}

// A Writer enters room
void downwritesema(struct rwsemaphore *rws) {
    semaacquire(rws); // acquire the lock
    while (rws->writers <= 0 || rws->readers <= 0)
        semasleep(rws); // wait till there is no readers and writers
    rws->writers--; // enter
    semarelease(rws); // release the lock
}

// A writer exits room
void upwritesema(struct rwsemaphore *rws) {
    semaacquire(rws); // acquire the lock
    rws->writers++; // enter
    wakeup(rws); // signal to sleeping threads
    semarelease(rws); // release the lock
}
