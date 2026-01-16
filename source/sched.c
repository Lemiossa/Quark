/**
 * sched.c
 * Criado por Matheus Leme Da Silva
 */
#include <asm.h>
#include <defs.h>
#include <pmm.h>
#include <sched.h>
#include <string.h>
#include <timer.h>

I32 current_pid = 0;
struct PCB *current_task;
struct PCB idle_task;

// Libera uma tarefa
void free_task(struct PCB *t) {
  t->prev->next = t->next;
  t->next->prev = t->prev;
  pmm_free_page(t->stack);
  pmm_free_page(t);
}

// Inicializa escalonador
// a partir daqui, o kernel é a task idle
void sched_init(void) {
  idle_task.pid = current_pid++;
  idle_task.ppid = -1;
  idle_task.status = TASK_READY;
  idle_task.next = &idle_task;
  idle_task.prev = &idle_task;
  idle_task.quantum = KERNEL_TASK_TIME_SLICE;
  strcpy(idle_task.name, "idle");

  current_task = &idle_task;
}

// Escalonador
struct regs *sched(struct regs *r) {
  current_task->r = r;
  current_task->ticks++;

  // Acordar tarefas em estado SLEEPING
  struct PCB *t = current_task;
  do {
    if (t->status == TASK_SLEEPING && ticks >= t->wake_up_tick)
      t->status = TASK_READY;
    t = t->next;
  } while (t != current_task);

  if (current_task->quantum != 0) {
    current_task->quantum--;
    return r;
  }

  do {
    current_task = current_task->next;
  } while (current_task->status != TASK_READY &&
           current_task->status != TASK_WAITING);

  current_task->quantum = KERNEL_TASK_TIME_SLICE;

  return current_task->r;
}

// Cria uma terefa e retorna o pid dela
// -1 para erro
I32 spawn(void (*task)(void), char *name) {
  CLI();
  if (!task)
    return -1;

  void *stack = pmm_alloc_page();
  if (!stack)
    return -1;

  void *esp = stack + PAGE_SIZE;

  struct PCB *pcb = (struct PCB *)pmm_alloc_page();
  if (!pcb) {
    pmm_free_page(stack);
    return -1;
  }

  esp -= sizeof(struct regs);
  struct regs *r = (struct regs *)esp;

  r->ds = KERNEL_DATA_SELECTOR;
  r->es = KERNEL_DATA_SELECTOR;
  r->fs = KERNEL_DATA_SELECTOR;
  r->gs = KERNEL_DATA_SELECTOR;

  r->eax = 0;
  r->ecx = 0;
  r->edx = 0;
  r->ebx = 0;
  r->esp = (U32)esp;
  r->ebp = 0;
  r->esi = 0;
  r->edi = 0;

  r->int_no = 32;
  r->err_code = 0;

  r->eip = (U32)task;
  r->cs = KERNEL_CODE_SELECTOR;
  r->eflags = 0x200;

  pcb->status = TASK_READY;
  pcb->pid = current_pid++;
  pcb->ppid = current_task->pid;
  pcb->r = r;
  pcb->quantum = KERNEL_TASK_TIME_SLICE;
  pcb->wake_up_tick = 0;
  pcb->exit_code = 0;

  pcb->prev = current_task;
  pcb->next = current_task->next;

  if (name)
    strcpy(pcb->name, name);

  current_task->next->prev = pcb;
  current_task->next = pcb;

  STI();
  return pcb->pid;
}

// Dorme N milisegundos na tarefa atual
// NOTE: A tarefa idle(kernel) NÃO pode usar sleep, isso vai TRAVAR O SISTEMA!
void sleep(U32 n) {
  current_task->status = TASK_SLEEPING;
  current_task->wake_up_tick = ticks + MS_TO_TICK(n);

  while (current_task->status != TASK_READY)
    HLT();
}

// Sai de uma tarefa
void exit(I32 code) {
  current_task->status = TASK_ZOMBIE;
  current_task->exit_code = code;

  // Se tiver filhos -> joga eles pro pai
  struct PCB *t = current_task;
  struct PCB *ft = current_task;
  do {
    if (t->ppid == current_task->pid) {
      t->ppid = ft->pid;
    }
    if (t->pid == current_task->ppid)
      ft = t;
    t = t->next;
  } while (t != current_task);

  for (;;)
    HLT();
}

// espera alguma tarefa filha terminar, destroi ela, coloca o exit_code em
// *status e retorna o pid da tarefa morta
I32 wait(I32 *code) {
  int has_child = 0;

  current_task->status = TASK_WAITING;
  while (1) {
    struct PCB *t = current_task;
    do {
      if (t->ppid == current_task->pid) {
        has_child = 1;
        if (t->status == TASK_ZOMBIE) {
          I32 pid = t->pid;
          I32 exit_code = t->exit_code;
          free_task(t);
          if (code)
            *code = exit_code;
          return pid;
        }
      }
      t = t->next;
    } while (t != current_task);

    if (!has_child) {
      current_task->status = TASK_READY;
      return -1;
    }
  }
}
