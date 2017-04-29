
#include "cmsis_os.h" // CMSIS RTOS header file
#include "Board_Led.h"


/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
void led_Thread1 (void const *argument);                             // thread function
void led_Thread2 (void const *argument);
osThreadId t_led1, t_led2;                                          // thread id
osThreadDef (led_Thread1, osPriorityAboveNormal, 1, 0);                   // thread object
osThreadDef (led_Thread2, osPriorityNormal, 1, 0);

osSemaphoreId sem1;									
osSemaphoreDef(sem1);

int Init_All (void) {
	
	sem1 = osSemaphoreCreate(osSemaphore(sem1), 0);

  t_led1 = osThreadCreate (osThread(led_Thread1), NULL);
  if (!t_led1) return(-1);
	
	t_led2 = osThreadCreate (osThread(led_Thread2), NULL);
	if (!t_led2) return(-1);
	 
  return(0);
}

void led_Thread1 (void const *argument) 
{
	for (;;) 
	{
		osSemaphoreWait(sem1, osWaitForever);
		LED_On(1);                          
		osDelay(1000);
		LED_Off(1);
	}
}

void led_Thread2 (void const *argument) 
{
	for (;;) 
	{
		LED_On(3);		
		osSemaphoreRelease(sem1);
		osDelay(1000);
		LED_Off(3);
		osDelay(1000);
	}
}
