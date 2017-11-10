/*
*********************************************************************************************************
*                                        uC/OS-III Benchmarks
*
*                          (c) Copyright 2009-2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        uC/OS-III Benchmarks
*
*                                           Renesas RIN32M3
*                                               on the
*
*                                             R-IN32M3-EC
*                                          Evaluation Board
*
* Filename      : bench_qpend.c
* Version       : V1.00
* Programmer(s) : JBL
*********************************************************************************************************
* Note(s)       : none.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <stdio.h>

#include  <cpu.h>
#include  <lib_mem.h>

#include  "bench.h"


extern  OS_TCB   Task1TCB;
extern  CPU_STK  Task1Stk[4096];

extern  OS_TCB   Task2TCB;
extern  CPU_STK  Task2Stk[4096];


static void Task1 (void);
static void Task2 (void);

static OS_Q queue;

static BENCH_TS_TMR t1, t2, t;

static CPU_INT32U iter_cnt;

extern BENCH_TS_TMR BenchRes[BENCH_CFG_ITER_CNT];

static BENCH_RES res;

void bench_qpend ()
{
    OS_ERR  os_err;

    OSQCreate(&queue, "", 10u, &os_err);

    BenchResInit(&res);

    res.BenchName = "Q Pend (Task-Task)";

    OSTaskCreate((OS_TCB     *)&Task1TCB,
                 (CPU_CHAR   *)"Task1",
                 (OS_TASK_PTR ) Task1,
                 (void       *) 0,
                 (OS_PRIO     ) 3,
                 (CPU_STK    *)&Task1Stk[0],
                 (CPU_STK_SIZE) 1024 / 10,
                 (CPU_STK_SIZE) 1024,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&os_err);

    OSTaskCreate((OS_TCB     *)&Task2TCB,
                 (CPU_CHAR   *)"Task2",
                 (OS_TASK_PTR ) Task2,
                 (void       *) 0,
                 (OS_PRIO     ) 4,
                 (CPU_STK    *)&Task2Stk[0],
                 (CPU_STK_SIZE) 1024 / 10,
                 (CPU_STK_SIZE) 1024,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&os_err);

    OSTaskDel(&Task1TCB, &os_err);
    OSTaskDel(&Task2TCB, &os_err);

    OSQDel(&queue, 0u, &os_err);

    BenchResFin(&res);
    BenchResPrint(&res);

}


static void Task1 (void)
{
    OS_ERR os_err;
    OS_MSG_SIZE msg_size;
    void *p_void;

    (void)&p_void;

    for(iter_cnt = 0; iter_cnt < BENCH_CFG_ITER_CNT + BENCH_CFG_WARMUP_ITER_CNT; iter_cnt++) {
        t1 = BENCH_TS_GET();
        p_void = OSQPend(&queue, 0u, 0u, &msg_size, DEF_NULL, &os_err);
    }

    OSTaskSuspend(DEF_NULL, &os_err);
}

static void Task2 (void)
{
    OS_ERR os_err;

    for(; iter_cnt < BENCH_CFG_ITER_CNT + BENCH_CFG_WARMUP_ITER_CNT; ) {
        t2 = BENCH_TS_GET();
        t = t2 - t1;
        if(iter_cnt >= BENCH_CFG_WARMUP_ITER_CNT) {
            BenchResUpdate(&res, t);
        }
        OSQPost(&queue, DEF_NULL, 32u, 0u, &os_err);
    }

    OSTaskSuspend(DEF_NULL, &os_err);
}