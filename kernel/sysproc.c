#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

extern struct proc proc[NPROC];

// Desc: Prints current system processes.
// Author: Josiah Brough
// SID: 22160417
uint64 
sys_ps(void) 
{
  struct proc *p;

  // header row
  printf("[PID]\t\t[STATE]\t\t[NAME]\t\t[SZ]\n");

  // loop through processes in table
  for (p = proc; p < &proc[NPROC]; p++) 
  {
    int pid = p->pid;

    // get state as a string
    char *state = "???";
    switch (p->state) 
    {
      case UNUSED: continue;
      case USED:      state = "used\t"; break;
      case SLEEPING:  state = "sleeping"; break;
      case RUNNABLE:  state = "runnable"; break;
      case RUNNING:   state = "running\t"; break;
      case ZOMBIE:    state = "zombie\t"; break;
    }

    // print process information
    printf("%d\t\t%s\t%s\t\t%d\n", pid, state, p->name, p->sz);
  }
   
  return 0;
}

// Desc: Prints current running system processes.
// Author: Josiah Brough
// SID: 22160417
uint64 
sys_psrunning(void) 
{
  struct proc *p;

  // header row
  printf("[PID]\t\t[STATE]\t\t[NAME]\t\t[SZ]\n");

  // loop through processes in table
  for (p = proc; p < &proc[NPROC]; p++) 
  {
    int pid = p->pid;

    // print running processes' information
    if (p->state == RUNNING) 
    {
      printf("%d\t\trunning\t\t%s\t\t%d\n", pid, p->name, p->sz);
    }
  }
   
  return 0;
}

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
