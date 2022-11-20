#ifndef MINI_OS_H_
#define MINI_OS_H_

#include <stdint.h>

void _minios_start(void);
void _minios_entint(void);

void svc_handler(void);
static void _schedule_svc(uint8_t svc_num, uint32_t *svc_args);

void dispatch(void);
void _schedule_time(void);

static void _task_init_all(void);
static void _task_init(int32_t id);

#endif
