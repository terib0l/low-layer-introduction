#ifndef TCB_H_
#define TCB_H_

#include <stdint.h>

#define TASK_NUM 10
#define TASK_STACK_SIZE 1024
#define TASK_PRI_NUM 3

typedef struct tcb {
  /* Stack Pointer of Each Tasks */
  uint32_t *p_sp;

  /* Previous and Next Task */
  struct tcb *p_prev;
  struct tcb *p_next;

  /* Task's Information that Id, Primary, State, Time-up */
  int32_t id;
  int32_t pri;
  int32_t state;
  uint32_t time_up;
} tcb_t;

static tcb_t _tcb_tbl[TASK_NUM];

/* For primary config */
static tcb_t _task_cfg[TASK_NUM];

/* Stack of Each Tasks */
static uint32_t _task_stk[TASK_NUM][TASK_STACK_SIZE];

enum {
  STATE_READY = 0,
  STATE_RUNNING,
  STATE_WAITING,
  STATE_DORMANT,
};

#endif
