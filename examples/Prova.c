#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h" // GPIO 
#include "freertos/task.h" // TASKS
#include "freertos/queue.h"


#define TIME_S 5 // 1 * SECOND
#define TIME_MS 1000 // ms
#define WORK_TIME (TIME_S*TIME_MS) 

/*
Renato dos Santos Monteiro
Matheus Arnaud Macambira Guedes

*/

static TaskHandle_t consumer_handle_task = NULL; // handle of pin 4
static TaskHandle_t signal_pin_handle_task = NULL; // handle of pin 4
static QueueHandle_t queue_handle = NULL; // handle of queue
static esp_timer_handle_t timer_handle; // handle of timer

// static const int pin_num_4 = 4; //  signal emmiter
// static const int pin_num_12 = 12; // signal emmiter

static const int pin_isr_num_12 = 12; // ISR 
static const int pin_isr_num_13 = 13; // ISR
static const int pin_isr_num_14 = 14; // ISR


typedef struct pin_ms{
    int pin;
    int ms;
} pin_ms_t;


// Signal pin
pin_ms_t pin_15 = {
    .pin = 15,
    .ms = 10000
};

// ISR pins
pin_ms_t pin_12 = {
    .pin = 12,
    .ms = 250150
};

pin_ms_t pin_13= {
    .pin = 13,
    .ms = 500300
};

pin_ms_t pin_14= {
    .pin = 14,
    .ms = 990600
};

// 12(signal) -> to pin -> 2/4


static void IRAM_ATTR gpioISR(void* arg){
    xQueueSendFromISR(queue_handle, arg, NULL);
}

void signalPin(void *arg){
    pin_ms_t pin_conf = * (pin_ms_t*)arg;
    printf("Signal Pin Emitter: %d\n", pin_conf.pin);
    while(1){
        gpio_set_level(pin_conf.pin, 1);
        vTaskDelay(pin_conf.ms/portTICK_PERIOD_MS);
        gpio_set_level(pin_conf.pin, 0);
    }
}

void stopTimer(int delay){
    vTaskDelay(delay / portTICK_PERIOD_MS);
    esp_timer_stop(timer_handle);
}

void setGPIOToZero(void){
    gpio_set_level(GPIO_NUM_4,  0);
    gpio_set_level(GPIO_NUM_2, 0);
    
}

static void setLevelMotor(void *args){
    static int level = 1;

    gpio_set_level(GPIO_NUM_4,  level);
    gpio_set_level(GPIO_NUM_2, !level);
    printf("level: %d\n",  level);
    level = !level;
}


static void queueConsumer(void *pvParameters){
    int isrReceivedFrom = -1 ;
    int motorCycle = 0;

    esp_timer_create_args_t timer_args = {
        .callback = &setLevelMotor,
        .name = "Set level",
        .arg = NULL
    };

    if(!esp_timer_create(&timer_args, &timer_handle)){
        printf("Timer created\n");
    }
    else{
        printf("Timer not created\n");
    }
    
    while (1){
        if(xQueueReceive(queue_handle, &isrReceivedFrom, portMAX_DELAY)){
            printf("ISR received from: %d\n", isrReceivedFrom);
            printf("WORK TIME SET: %d s\n", WORK_TIME/1000);

            if(isrReceivedFrom == 12){
                motorCycle = pin_12.ms;
                printf("Queeu consumer motorCycle %d\n", motorCycle);
                esp_timer_start_periodic(timer_handle, motorCycle);
                stopTimer(WORK_TIME);
                
            }
            if(isrReceivedFrom == 13){
                motorCycle = pin_13.ms;
                printf("Queeu consumer motorCycle %d\n", motorCycle);
                esp_timer_start_periodic(timer_handle, motorCycle);
                stopTimer(WORK_TIME);
            }
            if(isrReceivedFrom == 14){
                motorCycle = pin_14.ms;
                printf("Queeu consumer motor Cicle %d\n", motorCycle);
                esp_timer_start_periodic(timer_handle, motorCycle);
                stopTimer(WORK_TIME);
            }

           setGPIOToZero(); // Set to zero after work time, to avoid the motor stay working
        
        }else{
            printf("Waiting for ISR\n");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }



    }
    
}



void app_main(void)
{
    //  Create queue
    queue_handle = xQueueCreate(1, sizeof(int));

    // ISR pins 12, 13, 14
    gpio_config_t io_conf;


    
    io_conf.intr_type = GPIO_INTR_POSEDGE; //! IMPORTANT desable interruptions
    io_conf.mode = GPIO_MODE_INPUT; // !IMPORTANT set as input mode

    io_conf.pin_bit_mask = (1ULL << 12) | (1ULL << 13) | (1ULL << 14); 
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    
    // Activate by ISR pins 2 and 4, and the signal pin 15
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE; //! IMPORTANT: desable interruptions
    io_conf.mode = GPIO_MODE_OUTPUT; // ! IMPORTANT set as output mode

    io_conf.pin_bit_mask = (1ULL << 2) | (1ULL << 4) | (1ULL << 15); // set as output pin 4 and 12
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&io_conf);

    // ISR
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_12, gpioISR, (void*) &pin_isr_num_12);
    gpio_isr_handler_add(GPIO_NUM_13, gpioISR, (void*) &pin_isr_num_13);
    gpio_isr_handler_add(GPIO_NUM_14, gpioISR, (void*) &pin_isr_num_14);

    xTaskCreate(queueConsumer, "Queue Consumer", 2048,NULL, 2, &consumer_handle_task);
    xTaskCreate(signalPin, "signal pin", 2048, (void*)&pin_15 , 2, &signal_pin_handle_task);
}

