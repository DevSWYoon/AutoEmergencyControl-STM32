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
#include "my_hal.h"
#include "Queue.h"
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
static  OS_TCB   AppTaskPrintAccelTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  CPU_STK  AppTaskCollisionStk[APP_TASK_COLLISION_STK_SIZE];
static  CPU_STK  AppTaskSuddenAccelStk[APP_TASK_SUDDEN_ACCEL_STK_SIZE];
static  CPU_STK  AppTaskPrintDataStk[APP_TASK_PRINT_DATA_STK_SIZE];

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
static  void  AppTaskPrintData (void *p_art);

/*
*********************************************************************************************************
*                                     KERNEL OBJECT DEFINES
*********************************************************************************************************
*/

OS_Q   SuddenAccelMsgQ;
void * SuddenAccelMsgQPtrs[10];

static bool flag = 1;

OS_TMR CollisionTmr;
OS_TMR SuddenAccelTmr;
OS_TMR TimeoutPendingTmr;

OS_SEM AccelDataSEM;
void * SuddenAccelMsgQPtrs[10];
queue_structure AccelerationQ;


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
    CPU_TS      ts;


   (void)p_arg;

	
    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();                                                 /* Initialize Memory Management Module                  */

    init_queue(&AccelerationQ);
		
	
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
            
            
        
    BSP_LED_On(0);
    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        OSTimeDlyHMSM(0, 0, 0, 1000,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
			/*
				uint8_t c;
				MPU6050_ReadBit(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_MOT_THR, 7, &c);
				APP_TRACE_INFO(("%d\n\r", c));
				MPU6050_ReadBit(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_STATUS, 6, &c);
				APP_TRACE_INFO(("%d\n\r", c));
				MPU6050_ReadBit(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_MOT_DUR, 0, &c);
				APP_TRACE_INFO(("%d\n\r", c));
			*/
			  
			
		s16 data[6];
		int index;
			
		MPU6050_GetRawAccelGyro(&data[0]);
			
		OSSemPend(&AccelDataSEM,
							10,
							OS_OPT_PEND_NON_BLOCKING,
							&ts,
							&err);
											
		if(err == OS_ERR_NONE) {
			push(&AccelerationQ, &data[0]);
		}
					
		OSSemPost(&AccelDataSEM,
					OS_OPT_POST_1,
					&err);


		BSP_LED_Toggle(1);
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
								 
	
    OSTaskCreate((OS_TCB     *)&AppTaskPrintAccelTCB,                
                 (CPU_CHAR   *)"App Task Print Data",
                 (OS_TASK_PTR ) AppTaskPrintData,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_PRINT_DATA_PRIO,
                 (CPU_STK    *)&AppTaskPrintDataStk[0],
                 (CPU_STK_SIZE) APP_TASK_PRINT_DATA_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_PRINT_DATA_STK_SIZE,
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
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
    CPU_TS      ts;

    // Create the Accel Data Semaphore
    OSSemCreate((OS_SEM *)&AccelDataSEM,
                (CPU_CHAR *)"Accel Data Semaphore",
                (OS_SEM_CTR) 1,
                (OS_ERR *)&err);

    // Create the Collision Timer
    OSTmrCreate((OS_TMR              *)&CollisionTmr,
                (CPU_CHAR            *)"Collision Tmr",
                (OS_TICK              ) 0,
                (OS_TICK              ) 10,
                (OS_OPT               ) OS_OPT_TMR_PERIODIC,
                (OS_TMR_CALLBACK_PTR  ) BuzzerTmrCallback,
                (void                *) NULL,
                (OS_ERR              *)&err);

    // Create the Sudden Accel Timer
    OSTmrCreate((OS_TMR              *)&SuddenAccelTmr,
                (CPU_CHAR            *)"Sudden Accel Tmr",
                (OS_TICK              ) 0,
                (OS_TICK              ) 50,
                (OS_OPT               ) OS_OPT_TMR_PERIODIC,
                (OS_TMR_CALLBACK_PTR  ) BuzzerTmrCallback,
                (void                *) NULL,
                (OS_ERR              *)&err);

    // Create the Timeout Pending Timer
    OSTmrCreate((OS_TMR              *)&TimeoutPendingTmr,
                (CPU_CHAR            *)"Timeout Pending Tmr",
                (OS_TICK              ) 0,
                (OS_TICK              ) 100,
                (OS_OPT               ) OS_OPT_TMR_PERIODIC,
                (OS_TMR_CALLBACK_PTR  ) TimeoutPendingTmrCallback,
                (void                *) NULL,
                (OS_ERR              *)&err);
    
    // Create the Sudden Accel MsgQ
    OSQCreate((OS_Q *)&SuddenAccelMsgQ,
              (CPU_CHAR *)"Sudden Accel Msg Q",
              (OS_MSG_QTY) 10,
              (OS_ERR *)&err);
}

static void AppTaskPrintData(void *p_arg) {
	OS_ERR err;
    CPU_TS ts;
	  acceldata get_data = {0,};
	  acceldata total_sum = {0,};
		int i, j;
		double last_value = 0, total_value = 0;
		
		while(DEF_TRUE) {
				OSSemPend(&AccelDataSEM,
									0,
									OS_OPT_PEND_BLOCKING,
									&ts,
									&err);
			
				for(i = 0; i < MAX_QUEUE_SIZE; ++i) {
						pop(&AccelerationQ, &get_data);
						for(j = 0; j < 3; ++j) {
								total_sum[j] += get_data[j] / MAX_QUEUE_SIZE;
						}
				}
				
				OSSemPost(&AccelDataSEM,
									OS_OPT_POST_1,
									&err);
				
				total_value = sqrt(pow(total_sum[0], 2) + pow(total_sum[1], 2) + pow(total_sum[2], 2));
				APP_TRACE_INFO(("Velocity Data\n\r"));
				APP_TRACE_INFO(("X : %d | Y : %d | Z : %d\n\r", total_sum[0], total_sum[1], total_sum[2]));
				APP_TRACE_INFO(("Now Velocity Data : %f", total_value * 1));
				
				if(last_value < total_value /4) {
						OSTaskQPost((OS_TCB *)&AppTaskCollisionTCB, 0, 0, (OS_OPT)OS_OPT_POST_FIFO, (OS_ERR *)&err);
				}
				
				last_value = total_value;

                BSP_LED_Toggle(2);
		}
}


static void AppTaskCollision(void *p_arg) {
    OS_ERR err;
		OS_MSG_SIZE size;
		CPU_TS ts;
	  uint32_t i;

	
    while (DEF_TRUE) {
		void *p_msg = OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &size, &ts, &err);
			
        if (err == OS_ERR_NONE) {
            APP_TRACE_INFO(("Collision Detected!\n\r"));

            BSP_LED_On(0);
						
			OS_TmrStart(&CollisionTmr, &err);

            OSTimeDlyHMSM(0, 0, 10, 0,
                          OS_OPT_TIME_HMSM_STRICT,
                          &err);

            OS_TmrStop(&CollisionTmr, OS_OPT_TMR_NONE, NULL, &err);

			flag = 1;
        }

        BSP_LED_On(0);
        
    }
}

static void AppTaskSuddenAccel(void *p_arg) {
    OS_ERR err, err2;
	OS_MSG_SIZE size;
	CPU_TS ts;
    uint32_t i;
    
    while (DEF_TRUE) {
        void *p_msg = OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &size, &ts, &err);

        if (err == OS_ERR_TIMEOUT) {
            BSP_LED_On(0);
            
            OS_TmrStart(&TimeoutPendingTmr, &err2);

            // 3-second timeout-pending MsgQ "SuddenAccelMsgQ"
            void *msg = OSQPend(&SuddenAccelMsgQ, 3000, OS_OPT_PEND_BLOCKING, NULL, NULL, &err);

            OS_TmrStop(&TimeoutPendingTmr, OS_OPT_TMR_NONE, NULL, &err2);

            if (err == OS_ERR_NONE) {
                APP_TRACE_INFO(("Sudden Accel Aborted!\n\r"));
            } else if (err == OS_ERR_TIMEOUT) {
                APP_TRACE_INFO(("Sudden Accel Detected!\n\r"));
                RelayCutOff();
            }
        }

        BSP_LED_On(0);
    }
}

void EXTI1_IRQHandler(void) {
    OS_ERR err;
	
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        if(flag) {
            flag = 0;
			OSTaskQPost((OS_TCB *)&AppTaskCollisionTCB, 0, 0, (OS_OPT)OS_OPT_POST_FIFO, (OS_ERR *)&err);
        }


		EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

void EXTI4_IRQHandler(void) {
		OS_ERR err;
		
	
    if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
        if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == RESET) {
            OSQPost(&SuddenAccelMsgQ, (void *)0, 0, OS_OPT_POST_FIFO, &err);
            BSP_IntDis(BSP_INT_ID_EXTI9_5);
        } else  {
            BSP_IntEn(BSP_INT_ID_EXTI9_5);
        }

        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

void EXTI9_5_IRQHandler(void) {
    OS_ERR err;
    uint8_t c;
    
    if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
        OSTaskQPost((OS_TCB *)&AppTaskSuddenAccelTCB, 0, 0, (OS_OPT)OS_OPT_POST_FIFO, (OS_ERR *)&err);

        EXTI_ClearITPendingBit(EXTI_Line5);
    }
}