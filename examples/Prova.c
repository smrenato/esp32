#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h" // GPIO 
#include "freertos/task.h" // TASKS
#include "freertos/queue.h"



/*
Renato dos Santos Monteiro
Matheus Arnaud Macambira Guedes


*/

static TaskHandle_t motor_handle_task = NULL; // handle of pin 4



static QueueHandle_t queue_handle = NULL; // handle of queue

// static const int pin_num_4 = 4; //  signal emmiter
// static const int pin_num_12 = 12; // signal emmiter

static const int pin_isr_num_12 = 12; // ISR 
static const int pin_isr_num_13 = 13; // ISR
static const int pin_isr_num_14 = 14; // ISR


typedef struct pin_ms{
    int pin;
    int ms;
} pin_ms_t;

pin_ms_t pin_15 = {
    .pin = 15,
    .ms = 10000
};

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

static void motorTask(int motorCicle){
    int level = 0;

   
        printf("%d\n", motorCicle);
        gpio_set_level(GPIO_NUM_4, level);
        printf("Level:%d\n", level);
        gpio_set_level(GPIO_NUM_2,!level);
        vTaskDelay(250.150/ portTICK_PERIOD_MS);
        level = !level;
    
}



static int level = 0;

static void setLevelMotor(void *args){
    level = !level;
    gpio_set_level(GPIO_NUM_4, level);
    gpio_set_level(GPIO_NUM_2,!level);    
}

static void queueConsumer(void *pvParameters){
    int isrReceivedFrom = -1 ;
    int motorCicle = 0;
    int level = 0;

    esp_timer_handle_t timer;
    esp_timer_create_args_t timer_args = {
                .callback = &setLevelMotor,
                .name = "SEt level",
                .arg = NULL
            };


    esp_timer_create(&timer_args, &timer);
    
    while (1){
        if(xQueueReceive(queue_handle, &isrReceivedFrom, portMAX_DELAY)){
            printf("ISR received from %d\n", isrReceivedFrom);
            motorCicle = isrReceivedFrom == 12 ? pin_12.ms : -2;
            motorCicle = isrReceivedFrom == 13 ? pin_13.ms : -2;
            motorCicle = isrReceivedFrom == 14 ? pin_14.ms : -2;


            esp_timer_start_periodic(timer, motorCicle);
          
        }

        vTaskDelay(10000/ portTICK_PERIOD_MS);
    }
    
}



void app_main(void)
{

    queue_handle = xQueueCreate(5, sizeof(int));
    // PIN 12
    gpio_config_t io_conf;
    
    io_conf.intr_type = GPIO_INTR_POSEDGE; //! IMPORTANT desable interruptions
    io_conf.mode = GPIO_MODE_INPUT; // IMPORTANT set as output mode

    io_conf.pin_bit_mask = (1ULL << 12) | (1ULL << 13) | (1ULL << 14); 
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    
    
    gpio_config(&io_conf);

    
    //PIN 4 and 2
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE; // IMPORTANT: desable interruptions
    io_conf.mode = GPIO_MODE_OUTPUT; // IMPORTANT set as output mode

    io_conf.pin_bit_mask = (1ULL << 4) | (1ULL << 2) ; // set as output pin 4 and 12
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    
    gpio_config(&io_conf);

    // ISR
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_12, gpioISR, (void*) &pin_isr_num_12);
    gpio_isr_handler_add(GPIO_NUM_13, gpioISR, (void*) &pin_isr_num_13);
    gpio_isr_handler_add(GPIO_NUM_14, gpioISR, (void*) &pin_isr_num_14);

    xTaskCreate(queueConsumer, "motorTask", 2048,NULL, 2, &motor_handle_task);
}

