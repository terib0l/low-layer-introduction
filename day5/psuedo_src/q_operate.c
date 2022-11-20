#include "q_operate.h"

/* Add a task into last of Ready Que */
static void _q_add_ready(int32_t task_id)
{
  tcb_t *p_task = &_tcb_tbl[task_id];
  tcb_t *p_ptr = &_queue_ready[p_task->pri];

  while(p_ptr->p_next != 0) {
    p_ptr = p_ptr->p_next;
  }

  p_ptr->p_next = p_task;
  p_task->p_prev = p_ptr;
  p_task->p_next = 0;
}

/* Remove a task from Ready or Waiting Que */
static void _q_remove(int32_t task_id)
{
  tcb_t *p_task = &_tcb_tbl[task_id];

  p_task->p_prev->p_next = p_task->p_next;
  p_task->p_next->p_prev = p_task->p_prev;
  p_task->p_prev = 0;
  p_task->p_next = 0;
}

/* Retrieve the task which should execute next from Ready Que */
static int32_t _q_top_ready(void)
{
  int32_t pri;
  for (pri = 0; pri < TASK_PRI_NUM; pri++) {
    if (_queue_ready[pri].p_next != 0) {
      return _queue_ready[pri].p_next->id;
    }
  }
  return 0;
}
