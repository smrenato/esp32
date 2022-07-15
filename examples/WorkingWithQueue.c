#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define QUEUE1_DELAY 500
#define QUEUE2_DELAY 100

static QueueHandle_t queue_handle = NULL;

static TaskHandle_t send_to_queue_1_handle= NULL;
static TaskHandle_t send_to_queue_2_handle= NULL;
static TaskHandle_t consumer_queue_handle = NULL;


void sendToQueue1(void *parameters){
    int a = 1;
    while (1)
    {
        xQueueSend(queue_handle,&a,0);
        vTaskDelay(QUEUE1_DELAY / portTICK_PERIOD_MS);

    }
}

void sendToQueue2(void *parameters){
    int a = 2;

    while (1)
    {
        xQueueSend(queue_handle,&a,0);
        vTaskDelay(QUEUE2_DELAY  / portTICK_PERIOD_MS);

    }
}


void consumerQueue(void *parameters){
    int recebido;

    while (1)
    {
       if(xQueueReceive(queue_handle,&recebido,portMAX_DELAY)){
            printf("%d\n", recebido);
       }

    }
}


void app_main(void)
{   
    queue_handle = xQueueCreate(10,sizeof(int));

    xTaskCreate(sendToQueue1, "sendToQueue1", 2048, NULL, 1, &send_to_queue_1_handle);
    xTaskCreate(sendToQueue2, "sendToQueue2", 2048, NULL, 1, &send_to_queue_2_handle);
    xTaskCreate(consumerQueue, "consumerQueue", 2048, NULL, 1, &consumer_queue_handle);

   
}