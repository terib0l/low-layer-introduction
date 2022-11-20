#ifndef SYSCALL_H_
#define SYSCALL_H_

#define SVC_ACT_TSK 0
#define SVC_EXT_TSK 1
#define SVC_DLY_TSK 2

#include <stdint.h>

ER dly_tsk(int32_t dlytim)
{
  __asm("svc 1");
  return E_OK;
}

#endif
