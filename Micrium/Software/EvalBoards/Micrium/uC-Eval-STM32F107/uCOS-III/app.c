/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>
#include "USR_INIT.h"
#include <stdint.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;
static  OS_TCB   AppTaskCollisionTCB;
static  OS_TCB   AppTaskSuddenAccelTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  CPU_STK  AppTaskCollisionStk[APP_TASK_COLLISION_STK_SIZE];
static  CPU_STK  AppTaskSuddenAccelStk[APP_TASK_SUDDEN_ACCEL_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskCreate (void);
static  void  AppObjCreate  (void);
static  void  AppTaskStart  (void *p_arg);
static  void  AppTaskCollision  (void *p_arg);
static  void  AppTaskSuddenAccel  (void *p_arg);

/*
*********************************************************************************************************
*                                     KERNEL OBJECT DEFINES
*********************************************************************************************************
*/

OS_Q   SuddenAccelMsgQ;
void * SuddenAccelMsgQPtrs[10];

static bool flag = 1;


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;


    BSP_IntDisAll();                                            /* Disable all interrupts.                              */

    OSInit(&err);                                               /* Init uC/OS-III.                                      */
	

	
	

	  
	
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


   (void)p_arg;

	
    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();                                                 /* Initialize Memory Management Module                  */


		
		
	
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

    CPU_IntDisMeasMaxCurReset();

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
    BSP_Ser_Init(115200);                                       /* Enable Serial Interface                              */
#endif

		MPU6050_I2C_Init();
		
		MPU6050_Initialize();
		Enable_Motion_Interrupt(MPU6050_DEFAULT_ADDRESS, (uint8_t)MPU6050_Threshold);
		
		NVIC_Configuration();

		if(MPU6050_TestConnection()) {
			APP_TRACE_INFO(("YES"));
		} else {
			APP_TRACE_INFO(("NO"));
		}

    APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
    AppTaskCreate();                                            /* Create Application Tasks                             */
		
    APP_TRACE_INFO(("Creating Application Events...\n\r"));
    AppObjCreate();                                             /* Create Application Objects                           */
		
		
	 
    BSP_LED_Off(0);
    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        BSP_LED_Toggle(3);
			
        OSTimeDlyHMSM(0, 0, 0, 500,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
			
				uint8_t c;
				MPU6050_ReadBit(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_MOT_THR, 7, &c);
				APP_TRACE_INFO(("%d\n\r", c));
				MPU6050_ReadBit(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_STATUS, 6, &c);
				APP_TRACE_INFO(("%d\n\r", c));
				MPU6050_ReadBit(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_MOT_DUR, 0, &c);
				APP_TRACE_INFO(("%d\n\r", c));
			
			  
			
			s16 data[6];
			int index;
			
			
					MPU6050_GetRawAccelGyro(&data[0]);
					
					BSP_LED_Off(0);
			
					
					for(index = 0; index < 6; ++index) {
						APP_TRACE_INFO(("%d ", data[index]));
					}
				
				APP_TRACE_INFO(("\n\r"));
    }
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
    OS_ERR  err;
    
    OSTaskCreate((OS_TCB     *)&AppTaskCollisionTCB,                
								 (CPU_CHAR   *)"App Task Collision",
                 (OS_TASK_PTR ) AppTaskCollision,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_COLLISION_PRIO,
                 (CPU_STK    *)&AppTaskCollisionStk[0],
                 (CPU_STK_SIZE) APP_TASK_COLLISION_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_COLLISION_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
								 
    OSTaskCreate((OS_TCB     *)&AppTaskSuddenAccelTCB,                
                 (CPU_CHAR   *)"App Task Sudden Accel",
                 (OS_TASK_PTR ) AppTaskSuddenAccel,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_SUDDEN_ACCEL_PRIO,
                 (CPU_STK    *)&AppTaskSuddenAccelStk[0],
                 (CPU_STK_SIZE) APP_TASK_SUDDEN_ACCEL_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_SUDDEN_ACCEL_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
								 
								 
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION EVENTS
*
* Description:  This function creates the application kernel objects.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
    OS_ERR  err;
    
    OSQCreate((OS_Q *)&SuddenAccelMsgQ,
              (CPU_CHAR *)"Sudden Accel Msg Q",
              (OS_MSG_QTY) 10,
              (OS_ERR *)&err);
}


static void AppTaskCollision(void *p_arg) {
    OS_ERR err;
		OS_MSG_SIZE size;
		CPU_TS ts;
	  uint32_t i;

	
    while (DEF_TRUE) {
				BSP_LED_Off(0);

        void *p_msg = OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &size, &ts, &err);
			
        if (err == OS_ERR_NONE) {
            APP_TRACE_INFO(("Collision Detected!\n\r"));

            BSP_LED_On(0);
						
						for(i = 0; i < 50; ++i) {
							BSP_LED_Toggle(1);
							OSTimeDlyHMSM(0, 0, 0, 100,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
						}
						flag = 0;
        }
        
    }
}

static void AppTaskSuddenAccel(void *p_arg) {
    OS_ERR err;
		OS_MSG_SIZE size;
		CPU_TS ts;
    
    while (DEF_TRUE) {
        void *p_msg = OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &size, &ts, &err);

        if (err == OS_ERR_NONE) {
            APP_TRACE_INFO(("Sudden Accel Detected!\n\r"));

            // 3-second timeout-pending MsgQ "SuddenAccelMsgQ"
            void *msg = OSQPend(&SuddenAccelMsgQ, 3000, OS_OPT_PEND_BLOCKING, NULL, NULL, &err);

            if (err == OS_ERR_NONE) {
                APP_TRACE_INFO(("Sudden Accel Aborted!\n\r"));
            } else if (err == OS_ERR_TIMEOUT) {
                APP_TRACE_INFO(("Sudden Accel Detected!\n\r"));
            }

            BSP_LED_On(1);

        }
        
    }
}

void EXTI1_IRQHandler(void) {
    OS_ERR err;
	
	
    // MPU6050_1? INT ? ???? ??
    if (EXTI_GetITStatus(EXTI_Line1) != RESET && flag) {
//        OSTaskQPost((OS_TCB *)&AppTaskCollisionTCB, 0, 0, (OS_OPT)OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED, (OS_ERR *)&err);
			  OSTaskQPost((OS_TCB *)&AppTaskCollisionTCB, 0, 0, (OS_OPT)OS_OPT_POST_FIFO, (OS_ERR *)&err);
			  EXTI_ClearITPendingBit(EXTI_Line1);

        // MPU6050_1? INT ? ???? ??? ???

        // MPU6050_1? INT ? ???? ??
    }
}

void EXTI4_IRQHandler(void) {
		OS_ERR err;
		
	
    // MPU6050_1? INT ? ???? ??
    if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
			
//        OSTaskQPost((OS_TCB *)&AppTaskCollisionTCB, 0, 0, (OS_OPT)OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED, (OS_ERR *)&err);
				
			  OSTaskQPost((OS_TCB *)&AppTaskCollisionTCB, 0, 0, (OS_OPT)OS_OPT_POST_FIFO, (OS_ERR *)&err);
			  EXTI_ClearITPendingBit(EXTI_Line4);

        // MPU6050_1? INT ? ???? ??? ???

        // MPU6050_1? INT ? ???? ??
    }
}