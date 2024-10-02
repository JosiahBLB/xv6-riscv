// Header file for semaphore
#include "spinlock.h"

struct semaphore {
  int value;
  struct spinlock lk;
};

struct rwsemaphore {
  int readers; // readers counter [n, 1]
  int writers; // writers counter [0, 1]
  uint lk; // the lock [0, 1]
};

// Prototype of the three semaphore functions in semaphore.c
void initsema(struct semaphore*, int);
int downsema(struct semaphore*);
int upsema(struct semaphore*);

// Prototype of the five rwsemaphore functions in semaphore.c
void initrwsema(struct rwsemaphore *rws);
int downreadsema(struct rwsemaphore *rws);
int upreadsema(struct rwsemaphore *rws);
void downwritesema(struct rwsemaphore *rws);
void upwritesema(struct rwsemaphore *rws);
