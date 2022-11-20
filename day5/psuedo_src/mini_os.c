#include <cstdint>
#include <stdio.h>
#include <stdint.h>

#include "mini_os.h"
#include "q_operate.h"
#include "tcb.h"

void _minios_start(void)
{
  /* Initialization All TCB and Stack of Tasks */
  _task_init_all();

  /* Retrieve a task from first task of Ready Que */
  _sysinfo.running = _q_top_ready();

  /* Retrieve a task from first task of Ready Que */
  _tcb_tbl[_sysinfo.running].state = STATE_RUNNING;
  _task_start();
}

__attribute__((naked)) void _minios_entint(void)
{
  __asm("mrs r1, psp");

  __asm("mov %0, r1" : "=r"(_tcb_tbl[_sysinfo.running].p_sp) : :);

  __asm("bx lr");
}

void svc_handler(void)
{
  unsigned long *svc_args;
  char svc_num;

  _minios_entint();

  __asm("mrs r1, psp");

  /*
   * svc_argx[0] : R0
   * svc_argx[1] : R1
   * svc_argx[2] : R2
   * svc_argx[3] : R3
   * svc_argx[4] : R12
   * svc_argx[5] : R14(LR)
   * svc_argx[6] : R15(PC)
   */
  __asm("mov %0, r1" : "=r"(svc_args) : :);

  svc_num = ((char *)(svc_args[6]))[-2];

  _schedule_svc(svc_num, (uint32_t *)svc_args);
}

static void _schedule_svc(uint8_t svc_num, uint32_t *svc_args)
{
  switch(svc_num){
    /* */
    case SVC_DLY_TSK:
      _q_remove(_sysinfo.running);

      _tcb_tbl[_sysinfo.running].time_up = _sysinfo.systime + svc_args[0];
      _tcb_tbl[_sysinfo.running].satte = STATE_WAITING;
      _q_add_waiting(_sysinfo.running);
      break;
    /* */
  }

  _sysinfo.running = _q_top_ready();
  _tcb_tbl[_sysinfo.running].state = STATE_RUNNING;
}

__attribute__((naked)) void dispatch(void)
{
  __asm("ldr lr, =0xFFFFFFFD");

  __asm("mov r0, %0" : : "r"(_tcb_tbl[_sysinfo.running].p_sp) :);

  __asm("ldmfd r0!, {r4-r11}");
  __asm("msr psp, r0");

  __asm("bx lr");
}

void _schedule_time(void)
{
  tcb_t *p_task = _queue_waiting.p_next;
  while (p_task != 0) {
    if (p_task->time_up < _sysinfo.systime) {
      _q_remove(p_task->id);
      _q_add_ready(p_task->id);
    }
    p_task = p_task->p_next;
  }

  _sysinfo.running = _q_top_ready();
  _tcb_tbl[_sysinfo.running].state = STATE_RUNNING;
}

// static void _task_start(void)
// {
//   uint32_t func = _task_stk[_sysinfo.running][TASK_STACK_SIZE - 2];
//   func();
// }

static void _task_init_all(void)
{
  int32_t id;
  for (id = 0; id < TASK_NUM; id++) {
    _task_init(id);
    _q_add_ready(id);
  }
}

static void _task_init(int32_t id)
{
  /* タスクID */
  _tcb_tbl[id].id = id;
  /* 初期優先度の設定 */
  _tcb_tbl[id].pri = _task_cfg[id].pri;
  /* スタックポインタを設定 */
  /* 末尾から4*16Byteの一(ディスパッチ時に全レジスタを復元するため) */
  _tcb_tbl[id].p_sp = &_task_stk[id][TASK_STACK_SIZE - 16];

  /* レディ状態に設定 */
  _tcb_tbl[id].state = STATE_READY;

  /* xPSRの初期値：Thumb命令を使うので24ビットを1に設定 */
  /* 0b_0000_0000_1000_0000_0000_0000_0000_0000 == 0x_0100_0000 */
  /* (Cortex-MではThumb命令のみでARM命令は使用できない) */
  _task_stk[id][TASK_STACK_SIZE - 1] = 0x01000000;
  /* R15(PC)の初期値：task1の先頭アドレスを指定 */
  _task_stk[id][TASK_STACK_SIZE - 2] = (uint32_t)(_task_cfg[id].p_func);
}
