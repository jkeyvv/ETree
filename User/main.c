#include "main.h"
#include "Delay.h"
#include "Log.h"

#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t TaskTest;

void TestTask(void *d)
{
	for (;;)
	{
		vTaskDelay(1000);
		Log_Debug("TestTask");
		Log_Print();
	}
}

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	Delay_DWT_Init();
	Log_Init();

	xTaskCreate(TestTask, "TestTask", 100, (void *)NULL, 3, &TaskTest);

	vTaskStartScheduler();

	for (;;)
	{
	}
}
