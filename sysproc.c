#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"

int sys_settickets(void)
{
  int ticketnum;
  if(argint(0, &ticketnum) != 0 || ticketnum != 1) {
    return -1;
  }

  myproc()->ticketnum = ticketnum;
  acquire(&ptable.lock);
  ptable.proc[myproc()-ptable.proc].ticketnum = ticketnum;
  release(&ptable.lock);
  return 0;
}

int sys_getpinfo(void)
{
  acquire(&ptable.lock);
  struct pstat *procstat;
  struct proc *p;

  if( argptr(0, (void*)&procstat, sizeof(*procstat) ) < 0) 
    return -1;

  int ind = 0;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if(p->state != UNUSED)
    {
      procstat->inuse[ind] = 1;
      procstat->pid[ind] = p->pid;
      procstat->tickets[ind] = p->ticketnum;
      procstat->ticks[ind] = p->ticks;
    }
    ind++;
  }
  release(&ptable.lock);
  return 0;
}

int
sys_mprotect(void)
{
  int addr;
  int len = 0;
  if(argint(0, &addr)<0 || argint(1, &len)<0)
    return -1;
  
  struct proc *p = myproc();
  if(addr % PGSIZE != 0) // if addr is not page aligned
  {
    return -1;
  }
  if(addr+len*PGSIZE>p->sz) // if addr points to a region that is not currently a part of the address space
  {
    return -1;
  }
  if(len<=0) // if len is less than or equal to zero
  {
    return -1;
  }
}

int sys_munprotect(void)
{
  return 0;
}

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
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

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
