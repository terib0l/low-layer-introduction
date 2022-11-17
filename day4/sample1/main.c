#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <setjmp.h>

#ifdef __linux__
#undef setjmp
#endif

#define TCB_NUM 16

struct task_define {
  char *name;
  int (*mainfunc)(int argc, char *argv[]);
  int stack_size;
};

struct task_control_block {
  struct task_define *task;
  jmp_buf context;
} tcbs[TCB_NUM];

struct task_control_block *current;
struct task_control_block *max;

void schedule(void)
{
  current++;
  if (current == max)
    current = tcbs;
}

void dispatch(void)
{
  longjmp(current->context, 1);
}

void context_switch(void)
{
  if(setjmp(current->context) != 0)
    return;
  schedule();
  dispatch();
}

int task1_main(int argc, char *argv[])
{
  while(1) {
    fprintf(stderr, "task 1 %s %p\n", argv[0], current);
    context_switch();
    sleep(1);
  }

  return 0;
}

int task2_main(int argc, char *argv[])
{
  while(1) {
    fprintf(stderr, "task 2 %s %p\n", argv[0], current);
    context_switch();
    sleep(1);
  }

  return 0;
}

int task3_main(int argc, char *argv[])
{
  while(1) {
    fprintf(stderr, "task 3 %s %p\n", argv[0], current);
    context_switch();
    sleep(1);
  }

  return 0;
}

struct task_define tasks[] = {
  { "task1", task1_main, 4096*4 },
  { "task2", task2_main, 4096*4 },
  { "task3", task3_main, 4096*4 },
  { NULL, NULL, 0 }
};

void start(void)
{
  max = current;
  current = tcbs;
  dispatch();
}

void task_create(struct task_define *task);

void expand_stack(int size, struct task_define *task, char *dummy)
{
  char stack[1024];
  if (size < 0) {
    current++;
    task_create(++task);
  }
  expand_stack(size - sizeof(stack), task, stack);
}

void task_create(struct task_define *task)
{
  char *args[2];

  if (task->name == NULL)
    start();

  current->task = task;

  if (setjmp(current->context) == 0)
    expand_stack(task->stack_size, task, NULL);

  args[0] = current->task->name;
  args[1] = NULL;

  current->task->mainfunc(1, args);
}

int main()
{
  memset(tcbs, 0, sizeof(tcbs));
  current = tcbs;
  task_create(tasks);
  return 0;
}
