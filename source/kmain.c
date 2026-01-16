/**
 * kmain.c
 * Criado por Matheus Leme Da Silva
 */
#include <asm.h>
#include <defs.h>
#include <e820.h>
#include <ints/idt.h>
#include <ints/pic.h>
#include <io.h>
#include <panic.h>
#include <pmm.h>
#include <printk.h>
#include <sched.h>
#include <string.h>
#include <terminal.h>
#include <timer.h>

#ifndef VERSION
#define VERSION "UNDEFINED"
#endif

U64 total_ram = 0;
U64 total_usable_ram = 0;

char *status_strings[] = {"RUNNING", "READY", "SLEEPING", "ZOMBIE", "WAITING"};

void top(void) {
  for (;;) {
    sleep(1000);

    terminal_clear();
    printk("%-8s %-8s %-10s %-8s %s\n", "PID", "PPID", "STATUS", "TICKS",
           "NAME");

    struct PCB *t = current_task;
    do {
      char *status = status_strings[t->status];
      printk("%-8d %-8d %-10s %-8u %s\n", t->pid, t->ppid, status, t->ticks,
             t->name);
      t = t->next;
    } while (t != current_task);
  }
}

// Task que só dorme pra sempre
void task_sleeper(void) {
  for (;;) {
    sleep(1000);
  }
}

// Task que torra CPU
void task_cpu_hog(void) {
  volatile U64 x = 0;
  for (;;) {
    x++;
    if (x % 10000000 == 0)
      sleep(1);
  }
}

// Task que dorme tempos aleatórios
void task_random_sleep(void) {
  U32 t = 123456;
  for (;;) {
    t = t * 1103515245 + 12345;
    sleep((t % 3000) + 100);
  }
}

// Task suicida: divisão por zero
void task_div0(void) {
  sleep(2000);
  volatile int a = 10;
  volatile int b = 0;
  a = a / b;
  for (;;)
    ;
}

// Task contador infinito
void task_counter(void) {
  U64 counter = 0;
  for (;;) {
    counter++;
    if ((counter & 0xFFFFF) == 0)
      sleep(10);
  }
}

void init(void) {
  spawn(task_sleeper, "sleeper");
  spawn(task_cpu_hog, "cpu_hog");
  spawn(task_random_sleep, "rnd_sleep");
  spawn(task_div0, "div0");
  spawn(task_counter, "counter");
  spawn(top, "top");

  for (;;) {
    wait(NULL);
  }
}

// Func principal do kernel
void kmain(void) {
  terminal_init();
  terminal_set_color(VGA_WHITE, VGA_BLACK);
  terminal_clear();

  idt_init();
  pic_remap();
  e820_init();
  pmm_init();

  timer_init();
  sched_init();
  STI();

  printk("Total RAM: %llu, usable: %llu\n", total_ram, total_usable_ram);
  printk("Quark v%s kernel\n\n", VERSION);

  spawn(init, "init");

  for (;;) {
    HLT();
  }
}
