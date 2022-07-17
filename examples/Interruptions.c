#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h" // GPIO 
#include "freertos/task.h" // TASKS
#include "freertos/queue.h"

#define FREQ_TIME_X 1000
#define FREQ_TIME_Y 10000


static TaskHandle_t pin_4_freq_handle_task = NULL; // handle of pin 4
static TaskHandle_t pin_12_freq_handle_task = NULL; // handle of pin 12
static TaskHandle_t queue_consumer_handle_task = NULL; // handle of pin 12


static QueueHandle_t queue_handle = NULL; // handle of queue

static const int pin_num_4 = 4; //  signal emmiter
static const int pin_num_12 = 12; // signal emmiter

static const int pin_num_5 = 5; // ISR 
static const int pin_num_13 = 13; // ISR


// 4(signal) -> to pin -> 13(ISR_negedge)
// 12(signal) -> to pin -> 5(ISR_posedge)

// Queue 
static void queueConsumer(void *pvParameters){
    int isrReceivedFrom = -1 ;
    
    while (1){
        if(xQueueReceive(queue_handle, &isrReceivedFrom, portMAX_DELAY)){
            printf("ISR received from %d\n", isrReceivedFrom);
        }
    }
    
}


// ISR Handler

static void IRAM_ATTR gpioISR(void* arg){
    
    xQueueSendFromISR(queue_handle, arg, NULL);
    
}
 



// signal taks generator

void signal(const int pin, const int freq_time){
    
    while(1){
        printf("Pin: %d | ms: %d\n", pin, freq_time);
        gpio_set_level(pin, 1);
        vTaskDelay(freq_time / portTICK_PERIOD_MS);
        gpio_set_level(pin, 0);
        vTaskDelay(freq_time / portTICK_PERIOD_MS);
    }

}



void pin4Freq(void * parameters){
    const int pin = * (int*) parameters;
    signal(pin, FREQ_TIME_X);
}

void pin12Freq(void * parameters){
    const int pin = * (int*) parameters;
    signal(pin, FREQ_TIME_Y);
}




void app_main(void)
{

    //setup of output pins 
    
    gpio_config_t io_conf ; 

    queue_handle = xQueueCreate(5, sizeof(int));

    //PIN 4 and 12
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE; // IMPORTANT: desable interruptions
    io_conf.mode = GPIO_MODE_OUTPUT; // IMPORTANT set as output mode

    io_conf.pin_bit_mask = (1ULL << 4) | (1ULL << 12) ; // set as output pin 4 and 12
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    
    gpio_config(&io_conf);
    

    xTaskCreate(pin4Freq, "pin 4 Signal", 2048, (void*) &pin_num_4, 2, &pin_4_freq_handle_task);
    xTaskCreate(pin12Freq, "pin 12 Signal", 2048, (void*) &pin_num_12, 2, &pin_12_freq_handle_task);
    xTaskCreate(queueConsumer, "Queue consumer", 2048, NULL, 1, &queue_consumer_handle_task);


    // setup of ISR pins 

    // PIN 5
    io_conf.intr_type = GPIO_INTR_POSEDGE; // IMPORTANT desable interruptions
    io_conf.mode = GPIO_MODE_INPUT; // IMPORTANT set as output mode

    io_conf.pin_bit_mask = (1ULL << 5); // set as output pin 5 and 13
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    
    gpio_config(&io_conf);

    // PIN 13
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // IMPORTANT desable interruptions
    io_conf.mode = GPIO_MODE_INPUT; // IMPORTANT set as output mode

    io_conf.pin_bit_mask = (1ULL << 13); // set as output pin 5 and 13
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    
    gpio_config(&io_conf);

    // ISR
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_5, gpioISR, (void*) &pin_num_5);
    gpio_isr_handler_add(GPIO_NUM_13, gpioISR, (void*) &pin_num_13);


}

