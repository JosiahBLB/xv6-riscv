#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

// tell the compiler this is defined elsewhere
extern struct proc proc[NPROC];

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

uint64 sys_wait2(void) 
{
  uint64 addr, addr1, addr2, addr3;
  uint wtime, rtime, stime;
  if (argaddr(0, &addr) < 0)
    return -1;
  if (argaddr(1, &addr1) < 0) // user virtual memory
    return -1;
  if (argaddr(2, &addr2) < 0)
    return -1;
  if (argaddr(3, &addr3) < 0)
    return -1;
  int ret = wait2(addr, &rtime, &wtime, &stime);
  struct proc *p = myproc();
  if (copyout(p->pagetable, addr1, (char *)&wtime, sizeof(int)) < 0)
    return -1;
  if (copyout(p->pagetable, addr2, (char *)&rtime, sizeof(int)) < 0)
    return -1;
  if (copyout(p->pagetable, addr3, (char *)&stime, sizeof(int)) < 0)
    return -1;
  return ret;
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

// Desc: Sets the nice value of the specified process.
// Args: int pid: the pid of the target process.
//       int n:   the new nice value.
// Author: Josiah Brough
// SID: 22160417
uint64
sys_setnice(void)
{
  // retrive args
  int pid;
  int n;
  if (argint(0, &pid) < 0) return -1;
  if (argint(0, &n) < 0) return -1;

  // declare vars
  struct proc *p;
  uint64 prev_nice;

  // search proc for matching pid
  for(p = proc; p < &proc[NPROC]; p++) {
    if (p->pid == pid) { // update nice value
      prev_nice = p->nice;
      p->nice = n;
      return prev_nice;
    }
  }

  // return not found
  return -1;
}

// Desc: Gets the nice value of the specified process.
// Args: int pid: the pid of the target process.
// Author: Josiah Brough
// SID: 22160417
uint64
sys_getnice(void)
{
  // retrieve args
  int pid;
  if (argint(0, &pid) < 0) return -1;

  // declare var
  struct proc *p;

  // search proc for matching pid
  for(p = proc; p < &proc[NPROC]; p++) {
    if (p->pid == pid) {
      return p->nice; // return nice value
    }
  }

  // return not found
  return -1;
}
