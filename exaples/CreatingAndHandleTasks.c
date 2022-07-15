#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define FIB_DELAY 500
#define INTERVAL_DELAY 1000
#define SUSPEND_DELAY 3000
#define DATA_SIZE 10

static TaskHandle_t fib_handle_task = NULL;
static TaskHandle_t suspend_handle_task = NULL;


void fib(){
    int a = 0, b = 1, c = 0;
    while (1)
    {
        printf("%d\n", c); // 0 1
        if(c == 1){
            printf("1\n"); //1
        }

        c = a + b;  // c = 2
        a = b; 
        b = c;
        vTaskDelay(FIB_DELAY / portTICK_PERIOD_MS);
    }
}

void interval(){
    vTaskSuspend(fib_handle_task);
    vTaskDelay(INTERVAL_DELAY / portTICK_PERIOD_MS);
    vTaskResume(fib_handle_task);

}

void suspend(void *parameters){
    while(1){
        vTaskDelay(SUSPEND_DELAY/portTICK_PERIOD_MS);
        interval(NULL);

    }

}

// Sensor

int received[DATA_SIZE] = {1,3,2,3,1,4,5,4,6,6};
int toSend[DATA_SIZE] = {0,0,0,0,0,0,0,0,0,0};

void removeDuplicate(int* received){
    int i=0, j=0; 
    
    for ( i = 0; i < 10; i++)
    {
        for ( j = 0; j < 10; j++)
        {
            if (received[i] == received[j] && i != j )
            {
                                    
                // printf("%d", received[i]);
                // printf(" %d\n", received[j]);
                received[j]= -1;
            }

        }
    }

}

void prepareToSend(int* received, int* toSend){
    int i=0, j=0; 
    
    for ( i = 0; i < DATA_SIZE; i++)
    {
            toSend[i] = -1;
    }

    for ( i = 0; i < DATA_SIZE; i++)
    {
        if(received[i] != -1){
            toSend[j] = received[i];
            j++;
        }
    }
    
}

void app_main(void)
{   
    int i=0;
    
    xTaskCreate(fib, "fib", 2048, NULL, 2, &fib_handle_task);
    xTaskCreate(suspend, "suspend", 2048, NULL, 1, &suspend_handle_task);

    removeDuplicate(received);
    prepareToSend(received, toSend);
    
    for (i = 0; i < DATA_SIZE; i++)
    {
        printf("aqui %d\n", toSend[i]);
    }
}


