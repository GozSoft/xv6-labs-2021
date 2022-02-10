#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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
  backtrace();
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

uint64 sys_sigalarm(void)
{
  // printf("A");
  int ti;
  void (*fp)();

  if(argint(0, &ti) < 0) return -1;
  if(argaddr(1, (uint64*)&fp) < 0) return -1;

  myproc()->alarm_interval = ti;
  myproc()->handler = fp;

  return 0;
}

uint64 sys_sigreturn(void)
{
  struct proc *p = myproc();
  struct trapframe *tf = p->trapframe;
  struct trapframe *stf = &p->se_trapframe;
  tf->epc = stf->epc;
  tf->ra = stf->ra;
  tf->sp = stf->sp;
  tf->t0 = stf->t0;
  tf->t1 = stf->t1;
  tf->t2 = stf->t2;
  tf->s0 = stf->s0;
  tf->s1 = stf->s1;
  tf->a0 = stf->a0;
  tf->a1 = stf->a1;
  tf->a2 = stf->a2;
  tf->a3 = stf->a3;
  tf->a4 = stf->a4;
  tf->a5 = stf->a5;
  tf->a6 = stf->a6;
  tf->a7 = stf->a7;
  tf->s2 = stf->s2;
  tf->s3 = stf->s3;
  tf->s4 = stf->s4;
  tf->s5 = stf->s5;
  tf->s6 = stf->s6;
  tf->s7 = stf->s7;
  tf->s8 = stf->s8;
  tf->s9 = stf->s9;
  tf->s10 = stf->s10;
  tf->s11 = stf->s11;
  tf->t3 = stf->t3;
  tf->t4 = stf->t4;
  tf->t5 = stf->t5;
  tf->t6 = stf->t6;

  p->handler_flag = 0;
  return 0;
}
