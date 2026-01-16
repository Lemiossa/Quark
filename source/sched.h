/**
 * sched.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef SCHED_H
#define SCHED_H
#include <ints/idt.h>
#include <stdint.h>

#define TASK_RUNNING 0
#define TASK_READY 1
#define TASK_SLEEPING 2
#define TASK_ZOMBIE 3
#define TASK_WAITING 4

struct PCB {
  I32 pid;
  I32 ppid;
  U8 status;

  U32 ticks;
  U32 quantum;
  U32 wake_up_tick;

  I32 exit_code;

  void *stack;
  struct regs *r;

  struct PCB *prev;
  struct PCB *next;
  char name[32];
};

extern I32 current_pid;
extern struct PCB *current_task;
extern struct PCB idle_task;

void free_task(struct PCB *t);

void sched_init(void);
I32 spawn(void (*task)(void), char *name);
void sleep(U32 n);
void exit(I32 code);
I32 wait(I32 *code);

#endif // SCHED_H
