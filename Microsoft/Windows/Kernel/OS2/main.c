/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              uC/OS-II
*                                            EXAMPLE CODE
*
* Filename : main.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h>

#include  "app_cfg.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK  StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE];
#define TASK_STACKSIZE 2048

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void  task(void* pdata);
static  void  StartupTask (void  *p_arg);
void mywait(int tick);


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
*
* Notes       : none
*********************************************************************************************************
*/

int  main (void)
{
#if OS_TASK_NAME_EN > 0u
    CPU_INT08U  os_err;
#endif


    CPU_IntInit();

    Mem_Init();                                                 /* Initialize Memory Managment Module                   */
    CPU_IntDis();                                               /* Disable all Interrupts                               */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    OSInit();

    /*Initialize Output File*/
    OutFileInit();

    /*Input File*/
    InputFile();

                                                      /* Initialize uC/OS-II                                  */

    /*Dynamic Create the Stack size*/
    Task_STK = malloc(TASK_NUMBER * sizeof(int*));

    /* for each pointer, allocate storage for an array of ints*/
    int n;
    OS_TCB* ptcb;

    INT8U err;
    //R1 = OSMutexCreate(R1_PRIO, &err);
    //R2 = OSMutexCreate(R2_PRIO, &err);
    
    /*Create Task Set*/
    for (n = 0; n < TASK_NUMBER; n++){
        Task_STK[n] = malloc(TASK_STACKSIZE * sizeof(int));
        OSTaskCreateExt(task,                           
            &TaskParameter[n],
            &Task_STK[n][TASK_STACKSIZE - 1],
            TaskParameter[n].TaskPriority,
            TaskParameter[n].TaskID,
            &Task_STK[n][0],
            TASK_STACKSIZE,
            &TaskParameter[n],
            (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    }
    
    
    
    OSTimeSet(0);
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

    while (DEF_ON) {                                            /* Should Never Get Here.                               */
        ;
    }
}

void task(void* pdata) {
    INT8U err;
    task_para_set* task_data;
    task_data = pdata;

    while (1) 
    {
        
        if (task_data->R1LockTime != 0 && task_data->R2LockTime != 0) {
            if (task_data->R1LockTime < task_data->R2LockTime && task_data->R1UnlockTime < task_data->R2UnlockTime) {
                mywait(task_data->R1LockTime);

                OSSchedLock();
                printf("%2d\ttask %d get R1\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d get R1\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                mywait(task_data->R1UnlockTime - task_data->R1LockTime);
                printf("%2d\ttask %d release R1\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d release R1\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                OSSchedUnlock();

                mywait(task_data->R2LockTime - task_data->R1UnlockTime);

                OSSchedLock();
                printf("%2d\ttask %d get R2\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d get R2\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                mywait(task_data->R2UnlockTime - task_data->R2LockTime);
                printf("%2d\ttask %d release R2\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d release R2\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                OSSchedUnlock();

                mywait(task_data->TaskExecutionTime - task_data->R2UnlockTime);
            }
            else if (task_data->R1LockTime < task_data->R2LockTime && task_data->R1UnlockTime > task_data->R2UnlockTime) {
                mywait(task_data->R1LockTime);

                OSSchedLock();
                printf("%2d\ttask %d get R1\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d get R1\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                mywait(task_data->R2LockTime - task_data->R1LockTime);
                printf("%2d\ttask %d get R2\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d get R2\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                mywait(task_data->R2UnlockTime - task_data->R2LockTime);
                printf("%2d\ttask %d release R2\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d release R2\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                mywait(task_data->R1UnlockTime - task_data->R2UnlockTime);
                printf("%2d\ttask %d release R1\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d release R1\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                OSSchedUnlock();

                mywait(task_data->TaskExecutionTime - task_data->R1UnlockTime);
            }
            else if (task_data->R1LockTime > task_data->R2LockTime && task_data->R1UnlockTime > task_data->R2UnlockTime) {
                mywait(task_data->R2LockTime);

                OSSchedLock();
                printf("%2d\ttask %d get R2\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d get R2\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                mywait(task_data->R2UnlockTime - task_data->R2LockTime);
                printf("%2d\ttask %d release R2\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d release R2\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                OSSchedUnlock();

                mywait(task_data->R1LockTime - task_data->R2UnlockTime);

                OSSchedLock();
                printf("%2d\ttask %d get R1\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d get R1\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                mywait(task_data->R1UnlockTime - task_data->R1LockTime);
                printf("%2d\ttask %d release R1\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d release R1\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                OSSchedUnlock();

                mywait(task_data->TaskExecutionTime - task_data->R1UnlockTime);
            }
            else if (task_data->R1LockTime > task_data->R2LockTime && task_data->R1UnlockTime < task_data->R2UnlockTime) {
                mywait(task_data->R2LockTime);

                OSSchedLock();
                printf("%2d\ttask %d get R2\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d get R2\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                mywait(task_data->R1LockTime - task_data->R2LockTime);
                printf("%2d\ttask %d get R1\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d get R1\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                mywait(task_data->R1UnlockTime - task_data->R1LockTime);
                printf("%2d\ttask %d release R1\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d release R1\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                mywait(task_data->R2UnlockTime - task_data->R1UnlockTime);
                printf("%2d\ttask %d release R2\n", OSTimeGet(), task_data->TaskID);
                if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
                {
                    fprintf(Output_fp, "%2d\ttask %d release R2\n", OSTimeGet(), task_data->TaskID);
                    fclose(Output_fp);
                }
                OSSchedUnlock();

                mywait(task_data->TaskExecutionTime - task_data->R2UnlockTime);
            }
        }
        else if (task_data->R1LockTime != 0) {
            mywait(task_data->R1LockTime);

            OSSchedLock();
            printf("%2d\ttask %d get R1\n", OSTimeGet(), task_data->TaskID);
            if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
            {
                fprintf(Output_fp, "%2d\ttask %d get R1\n", OSTimeGet(), task_data->TaskID);
                fclose(Output_fp);
            }
            
            mywait(task_data->R1UnlockTime - task_data->R1LockTime);
 
            printf("%2d\ttask %d release R1\n", OSTimeGet(), task_data->TaskID);
            if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
            {
                fprintf(Output_fp, "%2d\ttask %d release R1\n", OSTimeGet(), task_data->TaskID);
                fclose(Output_fp);
            }
            OSSchedUnlock();

            mywait(task_data->TaskExecutionTime - task_data->R1UnlockTime);
        }
        else if (task_data->R2LockTime != 0) {
            mywait(task_data->R2LockTime);

            OSSchedLock();
            printf("%2d\ttask %d get R2\n", OSTimeGet(), task_data->TaskID);
            if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
            {
                fprintf(Output_fp, "%2d\ttask %d get R2\n", OSTimeGet(), task_data->TaskID);
                fclose(Output_fp);
            }
            mywait(task_data->R2UnlockTime - task_data->R2LockTime);
            printf("%2d\ttask %d release R2\n", OSTimeGet(), task_data->TaskID);
            if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0)
            {
                fprintf(Output_fp, "%2d\ttask %d release R2\n", OSTimeGet(), task_data->TaskID);
                fclose(Output_fp);
            }
            OSSchedUnlock();

            mywait(task_data->TaskExecutionTime - task_data->R2UnlockTime);
        }
        
    }
}

void mywait(int tick) {
#if OS_CRITICAL_METHOD==3
    OS_CPU_SR cpu_sr = 0;
#endif
    int now, exit;
    OS_ENTER_CRITICAL();
    now = OSTimeGet();
    exit = now + tick;
    //printf("exit: %d\n", exit);
    OS_EXIT_CRITICAL();
    while (1) {
        if (exit <= OSTimeGet()) {
            break;
        }
    }
}

/*
*********************************************************************************************************
*                                            STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'StartupTask()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  StartupTask (void *p_arg)
{
   (void)p_arg;

    OS_TRACE_INIT();                                            /* Initialize the uC/OS-II Trace recorder               */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    
    APP_TRACE_DBG(("uCOS-III is Running...\n\r"));

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        OSTimeDlyHMSM(0u, 0u, 1u, 0u);
		APP_TRACE_DBG(("Time: %d\n\r", OSTimeGet()));
    }
}

