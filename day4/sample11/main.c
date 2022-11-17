#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>

#ifdef __linux__
#undef setjmp
#endif

#define TCB_NUM 16
#define READYQUE_NUM 32

struct task_define {
  char *name;
  int priority;
  int (*mainfunc)(int argc, char *argv[]);
  int stack_size;
};

typedef enum {
    TCB_STATUS_NONE = 0,
    TCB_STATUS_ACTIVE,
    TCB_STATUS_SLEEP,
    TCB_STATUS_EXITED,
} tcb_status_t;

typedef enum {
    TCB_SYSCALL_EXIT,
    TCB_SYSCALL_SLEEP,
    TCB_SYSCALL_WAKEUP,
    TCB_SYSCALL_SETPRI,
    TCB_SYSCALL_SEND,
    TCB_SYSCALL_RECV,
    TCB_SYSCALL_MEMENQ,
    TCB_SYSCALL_MEMDEQ,
} tcb_syscall_t;

struct task_control_block {
  struct task_control_block *next;
  struct task_define *task;
  int priority;
  tcb_status_t status;
  int exit_code;
  struct {
    tcb_syscall_t code;
    long args[8];
    long ret;
  } syscall;
  jmp_buf context;
} tcbs[TCB_NUM];

struct task_ready_queue {
  struct task_control_block *head;
  struct task_control_block *tail;
} readyque[READYQUE_NUM];

unsigned int readyque_bitmap = 0;

struct task_control_block *current;

struct msgbuf {
  struct msgbuf *next;
  char buffer[0];
};

struct msgbox {
  struct task_control_block *receiver;
  struct msgbuf *head;
  struct msgbuf *tail;
};

typedef enum {
  MSGBOX_ID_SAMPLE = 0,
  MSGBOX_ID_NUM
} msgbox_id_t;

struct msgbox msgboxes[MSGBOX_ID_NUM];

#define MEMBUF_SIZE 64
#define MEMBUF_NUM  64

struct membuf {
  union {
    struct membuf *next;
    char buffer[MEMBUF_SIZE];
  } u;
};

struct membuf *memque = NULL;
struct membuf membufs[MEMBUF_NUM];

struct task_control_block *readyque_deq(int priority)
{
  struct task_ready_queue *q = &readyque[priority];
  struct task_control_block *tcb;

  tcb = q->head;
  if (tcb) {
    q->head = tcb->next;
    if (q->head == NULL) {
      q->tail = NULL;
      readyque_bitmap &= ~(1 << priority);
    }
    tcb->next = NULL;
  }

  return tcb;
}

void readyque_enq(struct task_control_block *tcb)
{
  struct task_ready_queue *q = &readyque[tcb->priority];

  tcb->next = NULL;

  if (q->tail) {
    q->tail->next = tcb;
  } else {
    q->head = tcb;
    readyque_bitmap |= (1 << tcb->priority);
  }
  q->tail = tcb;
}

int bitsearch(unsigned int bitmap)
{
  int n = 0;
  static int searchmap[] = {
    -29, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
  };

  if (!(bitmap & 0xffff)) {
    bitmap >>= 16;
    n += 16;
  }
  if (!(bitmap & 0xff)) {
    bitmap >>= 8;
    n += 8;
  }
  if (!(bitmap & 0xf)) {
    bitmap >>= 4;
    n += 4;
  }

  return searchmap[bitmap & 0xf] + n;
}

void schedule(void)
{
  int n = bitsearch(readyque_bitmap);
  if (n >= 0) {
    current = readyque_deq(n);
    return;
  }
  exit(0);
}

void dispatch(void)
{
  longjmp(current->context, 1);
}

void context_switch(void)
{
  if (setjmp(current->context) != 0)
    return;
  if (current->status == TCB_STATUS_ACTIVE)
    readyque_enq(current);
  schedule();
  dispatch();
}

int syscall_exit(int code)
{
  current->syscall.code = TCB_SYSCALL_EXIT;
  current->syscall.args[0] = code;
  kill(getpid(), SIGSYS);
  return current->syscall.ret;
}

int syscall_sleep(void)
{
  current->syscall.code = TCB_SYSCALL_SLEEP;
  kill(getpid(), SIGSYS);
  return current->syscall.ret;
}

int syscall_wakeup(struct task_control_block *tcb)
{
  current->syscall.code = TCB_SYSCALL_WAKEUP;
  current->syscall.args[0] = (long)tcb;
  kill(getpid(), SIGSYS);
  return current->syscall.ret;
}

int syscall_setpri(int priority)
{
  current->syscall.code = TCB_SYSCALL_SETPRI;
  current->syscall.args[0] = priority;
  kill(getpid(), SIGSYS);
  return current->syscall.ret;
}

int syscall_send(msgbox_id_t id, struct msgbuf *mp)
{
  current->syscall.code = TCB_SYSCALL_SEND;
  current->syscall.args[0] = id;
  current->syscall.args[1] = (long)mp;
  kill(getpid(), SIGSYS);
  return current->syscall.ret;
}

struct msgbuf *syscall_recv(msgbox_id_t id)
{
  current->syscall.code = TCB_SYSCALL_RECV;
  current->syscall.args[0] = id;
  kill(getpid(), SIGSYS);
  return (struct msgbuf *)current->syscall.ret;
}

int syscall_memenq(struct membuf *mb)
{
  current->syscall.code = TCB_SYSCALL_MEMENQ;
  current->syscall.args[0] = (long)mb;
  kill(getpid(), SIGSYS);
  return current->syscall.ret;
}

struct membuf *syscall_memdeq(void)
{
  current->syscall.code = TCB_SYSCALL_MEMDEQ;
  kill(getpid(), SIGSYS);
  return (struct membuf *)current->syscall.ret;
}

int task1_main(int argc, char *argv[])
{
  int i;
  struct msgbuf *mp;

  for (i = 0; i < 20; i++) {
    fprintf(stderr, "task 1 %s %p %d\n", argv[0], current, i);
    sleep(1);
    if (i == 10) {
      mp = (struct msgbuf *)syscall_memdeq();
      strcpy(mp->buffer, "message from task1");
      fprintf(stderr, "message send %p\n", mp);
      syscall_send(MSGBOX_ID_SAMPLE, mp);
    }
  }

  return 0;
}

int task2_main(int argc, char *argv[])
{
  int i;

  for (i = 0; i < 3; i++) {
    fprintf(stderr, "task 2 %s %p %d\n", argv[0], current, i);
    sleep(1);
  }

  return 0;
}

int task3_main(int argc, char *argv[])
{
  int i;
  struct msgbuf *mp;

  for (i = 0; i < 5; i++) {
    fprintf(stderr, "task 3 %s %p\n", argv[0], current);
    if (i == 1) {
      mp = syscall_recv(MSGBOX_ID_SAMPLE);
      fprintf(stderr, "message recv %p %s %d\n", mp, mp->buffer, i);
      syscall_memenq((struct membuf *)mp);
    }
    sleep(1);
  }

  return 0;
}

struct task_define tasks[] = {
  { "task1", 16, task1_main, 4096*4 },
  { "task2", 16, task2_main, 4096*4 },
  { "task3", 16, task3_main, 4096*4 },
  { NULL, -1, NULL, 0 }
};

void sigint_handler(int value)
{
  context_switch();
}

void sigalrm_handler(int value)
{
  alarm(2);
  context_switch();
}

void sigsegv_handler(int value)
{
  current->status = TCB_STATUS_SLEEP;
  context_switch();
}

long syscall_exit_proc(long args[])
{
  current->status = TCB_STATUS_EXITED;
  current->exit_code = args[0];
  return 0;
}

long syscall_sleep_proc(long args[])
{
  current->status = TCB_STATUS_SLEEP;
  return 0;
}

long syscall_wakeup_proc(long args[])
{
  struct task_control_block *tcb;
  tcb = (struct task_control_block *)args[0];
  tcb->status = TCB_STATUS_ACTIVE;
  readyque_enq(tcb);
  return 0;
}

long syscall_setpri_proc(long args[])
{
  current->priority = args[0];
  return 0;
}

struct msgbuf *recvmsg(struct msgbox *mboxp)
{
  struct msgbuf *mp;

  mp = mboxp->head;
  mboxp->head = mp->next;
  if (mboxp->head == NULL)
    mboxp->tail = NULL;
  mp->next = NULL;

  return mp;
}

long syscall_send_proc(long args[])
{
  struct msgbox *mboxp = &msgboxes[args[0]];
  struct msgbuf *mp = (struct msgbuf *)args[1];

  mp->next = NULL;

  if (mboxp->tail) {
    mboxp->tail->next = mp;
  } else {
    mboxp->head = mp;
  }
  mboxp->tail = mp;

  if (mboxp->receiver) {
    mp = recvmsg(mboxp);
    mboxp->receiver->syscall.ret = (long)mp;
    mboxp->receiver->status = TCB_STATUS_ACTIVE;
    readyque_enq(mboxp->receiver);
    mboxp->receiver = NULL;
  }

  return 0;
}

long syscall_recv_proc(long args[])
{
  struct msgbox *mboxp = &msgboxes[args[0]];
  struct msgbuf *mp;

  if (mboxp->receiver)
    return 0;

  mboxp->receiver = current;

  if (mboxp->head == NULL) {
    current->status = TCB_STATUS_SLEEP;
    return 0;
  }

  mp = recvmsg(mboxp);
  mboxp->receiver = NULL;

  return (long)mp;
}

void membuf_enq(struct membuf *mb)
{
  mb->u.next = memque;
  memque = mb;
}

struct membuf *membuf_deq(void)
{
  struct membuf *mb = memque;
  if (memque)
    memque = memque->u.next;
  return mb;
}

void membuf_init(void)
{
  int i;
  memset(membufs, 0, sizeof(membufs));
  for (i = 0; i < MEMBUF_NUM; i++)
    membuf_enq(&membufs[i]);
}

long syscall_memenq_proc(long args[])
{
  membuf_enq((struct membuf *)args[0]);
  return 0;
}

long syscall_memdeq_proc(long args[])
{
  return (long)membuf_deq();
}

typedef long (*syscall_proc_t)(long args[]);

syscall_proc_t syscall_table[] = {
  syscall_exit_proc,
  syscall_sleep_proc,
  syscall_wakeup_proc,
  syscall_setpri_proc,
  syscall_send_proc,
  syscall_recv_proc,
  syscall_memenq_proc,
  syscall_memdeq_proc,
};

void sigsys_handler(int value)
{
  syscall_proc_t proc;
  proc = syscall_table[current->syscall.code];
  current->syscall.ret = proc(current->syscall.args);
  context_switch();
}

void start(void)
{
  schedule();

  signal(SIGINT, sigint_handler);
  signal(SIGALRM, sigalrm_handler);
  signal(SIGBUS, sigsegv_handler);
  signal(SIGSEGV, sigsegv_handler);
  signal(SIGSYS, sigsys_handler);

  alarm(2);

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
  current->priority = task->priority;
  current->status = TCB_STATUS_ACTIVE;
  readyque_enq(current);

  if (setjmp(current->context) == 0)
    expand_stack(task->stack_size, task, NULL);

  args[0] = current->task->name;
  args[1] = NULL;

  syscall_exit(current->task->mainfunc(1, args));
}

int main(void)
{
  memset(tcbs, 0, sizeof(tcbs));
  memset(readyque, 0, sizeof(readyque));
  memset(msgboxes, 0, sizeof(msgboxes));
  membuf_init();
  current = tcbs;
  task_create(tasks);
  return 0;
}
