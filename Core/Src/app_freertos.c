/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
const uint8_t phrase[12] = "Ciao Mondo!";
const uint8_t test_send_string[128] = "Questa stringa viene usata per vedere come funziona il sincronismo tra task.\r\n";
char shared_mem;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
uint32_t defaultTaskBuffer[ 128 ];
osStaticThreadDef_t defaultTaskControlBlock;
osThreadId producerHandle;
uint32_t producerBuffer[ 128 ];
osStaticThreadDef_t producerControlBlock;
osThreadId consumerHandle;
uint32_t consumerBuffer[ 128 ];
osStaticThreadDef_t consumerControlBlock;
osMessageQId charQueueHandle;
uint8_t charQueueBuffer[ 1 * sizeof( uint8_t ) ];
osStaticMessageQDef_t charQueueControlBlock;
osMutexId sharedMemMutexHandle;
osStaticMutexDef_t sharedMemMutexControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartProducer(void const * argument);
void StartConsumer(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of sharedMemMutex */
  osMutexStaticDef(sharedMemMutex, &sharedMemMutexControlBlock);
  sharedMemMutexHandle = osMutexCreate(osMutex(sharedMemMutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of charQueue */
  osMessageQStaticDef(charQueue, 1, uint8_t, charQueueBuffer, &charQueueControlBlock);
  charQueueHandle = osMessageCreate(osMessageQ(charQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadStaticDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128, defaultTaskBuffer, &defaultTaskControlBlock);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of producer */
  osThreadStaticDef(producer, StartProducer, osPriorityBelowNormal, 0, 128, producerBuffer, &producerControlBlock);
  producerHandle = osThreadCreate(osThread(producer), NULL);

  /* definition and creation of consumer */
  osThreadStaticDef(consumer, StartConsumer, osPriorityAboveNormal, 0, 128, consumerBuffer, &consumerControlBlock);
  consumerHandle = osThreadCreate(osThread(consumer), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	/* Infinite loop */
	int i;
	for(;;)
	{
		osDelay(1000);
		LL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
		for (i=0; i<sizeof(phrase)-1; i++) {
			//    	while (!LL_USART_IsActiveFlag_TXE(USART1));
			//    	LL_USART_TransmitData8(USART1, phrase[i]);
		}
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartProducer */
/**
* @brief Function implementing the producer thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartProducer */
void StartProducer(void const * argument)
{
  /* USER CODE BEGIN StartProducer */
//#define USE_NO_CONTROLS
//#define USE_MUTEX
//#define USE_MUTEX_AND_GLOBAL_MEM_CONDITION
//#define USE_MUTEX_AND_QUEUE
	/* Infinite loop */
	int k;
	char* p;

#ifdef USE_NO_CONTROLS
	for(;;)
	{
		osDelay(1);
		k = strlen((const char *)test_send_string);
		p = (char *)&test_send_string[0];
		while(k) {
			shared_mem = *p;
			p++;
			k--;
			osDelay(1);
		}
	}
#endif

#ifdef USE_MUTEX
	for(;;)
	{
		osDelay(1);
		k = strlen((const char *)test_send_string);
		p = (char *)&test_send_string[0];
		while(k) {
			if (osOK == osMutexWait(sharedMemMutexHandle, portMAX_DELAY)) {
				shared_mem = *p;
				p++;
				k--;
				osMutexRelease(sharedMemMutexHandle);
			}
			osDelay(1);
		}
	}
#endif

#ifdef USE_MUTEX_AND_GLOBAL_MEM_CONDITION
	shared_mem = 0x00;
	for(;;)
	{
		osDelay(1);
		k = strlen((const char *)test_send_string);
		p = (char *)&test_send_string[0];
		while(k) {
			if (osOK == osMutexWait(sharedMemMutexHandle, portMAX_DELAY)) {
				if (shared_mem == 0x00) {
					shared_mem = *p;
					p++;
					k--;
				}
				osMutexRelease(sharedMemMutexHandle);
			}
			osDelay(10);
		}
	}
#endif

#ifdef USE_MUTEX_AND_QUEUE
	for(;;)
	{
		osDelay(1);
		k = strlen((const char *)test_send_string);
		p = (char *)&test_send_string[0];
		while(k) {
			if (osOK == osMutexWait(sharedMemMutexHandle, portMAX_DELAY)) {
				//			shared_mem = *p;
				if (osOK == osMessagePut(charQueueHandle, *p, 0)) {
					p++;
					k--;
				}
				osMutexRelease(sharedMemMutexHandle);
			}
			osDelay(1);
		}
	}
#endif
  /* USER CODE END StartProducer */
}

/* USER CODE BEGIN Header_StartConsumer */
/**
* @brief Function implementing the consumer thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartConsumer */
void StartConsumer(void const * argument)
{
  /* USER CODE BEGIN StartConsumer */
	/* Infinite loop */
#ifdef USE_NO_CONTROLS
	for(;;)
	{
		osDelay(10);
		LL_USART_TransmitData8(USART1, shared_mem);
	}
#endif

#ifdef USE_MUTEX
	for(;;)
	{
		osDelay(10);
		if (osOK == osMutexWait(sharedMemMutexHandle, portMAX_DELAY)) {
			LL_USART_TransmitData8(USART1, shared_mem);
			osMutexRelease(sharedMemMutexHandle);
		}
	}
#endif

#ifdef USE_MUTEX_AND_GLOBAL_MEM_CONDITION
	for(;;)
	{
		osDelay(1);
		if (shared_mem) {
			if (osOK == osMutexWait(sharedMemMutexHandle, portMAX_DELAY)) {
				LL_USART_TransmitData8(USART1, shared_mem);
				shared_mem = 0x00;
				osMutexRelease(sharedMemMutexHandle);
			}
		}
	}
#endif

#ifdef USE_MUTEX_AND_QUEUE
	osEvent ev;
	for(;;)
	{
		osDelay(10);
		if (osOK == osMutexWait(sharedMemMutexHandle, portMAX_DELAY)) {
			ev = osMessageGet(charQueueHandle, 0);
			if (osEventMessage == ev.status) {
				LL_USART_TransmitData8(USART1, ev.value.v);
			}
			osMutexRelease(sharedMemMutexHandle);
		}
	}
#endif
  /* USER CODE END StartConsumer */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
