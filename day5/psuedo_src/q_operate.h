#ifndef Q_OPERATE_H_
#define Q_OPERATE_H_

#include "tcb.h"

/* Ready and Waiting Que */
static tcb_t _queue_ready[TASK_PRI_NUM];
static tcb_t _queue_waiting;

static void _q_add_ready(int32_t task_id);
static void _q_add_waiting(void);
static void _q_remove(int32_t task_id);
static int32_t _q_top_ready(void);

#endif
