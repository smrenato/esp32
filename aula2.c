#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define QUEUE_DELAY 500
#define QUEUE2_DELAY 100

static QueueHandle_t fila = NULL;

static TaskHandle_t send_to_queue_1_handle= NULL;
static TaskHandle_t send_to_queue_2_handle= NULL;
static TaskHandle_t consumer_queue_handle = NULL;


void send_to_queue_1(void *parameters){
    int a = 1;
    while (1)
    {
        xQueueSend(fila,&a,0);
        vTaskDelay(QUEUE_DELAY / portTICK_PERIOD_MS);

    }
}

void send_to_queue_2(void *parameters){
    int a = 2;

    while (1)
    {
        xQueueSend(fila,&a,0);
        vTaskDelay(QUEUE2_DELAY  / portTICK_PERIOD_MS);

    }
}


void consumer_queue(void *parameters){
    int recebido;

    while (1)
    {
       if(xQueueReceive(fila,&recebido,portMAX_DELAY)){
            printf("%d\n", recebido);
       }

    }
}


void app_main(void)
{   
    fila = xQueueCreate(10,sizeof(int));

    xTaskCreate(send_to_queue_1, "send_to_queue_1", 2048, NULL, 1, &send_to_queue_1_handle);
    xTaskCreate(send_to_queue_2, "send_to_queue_2", 2048, NULL, 1, &send_to_queue_2_handle);
    xTaskCreate(consumer_queue, "consumer_queue", 2048, NULL, 1, &consumer_queue_handle);

   
}