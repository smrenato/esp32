#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define TIMER_DELAY 100000

static int blinkSwitch = 0;

void blinkHandle(void *args){
    blinkSwitch = !blinkSwitch;
    
    gpio_set_level(GPIO_NUM_2, blinkSwitch);

}

void app_main(void)
{


    gpio_config_t io_conf ;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE; // IMPORTANTE: desable interruptions
    io_conf.mode = GPIO_MODE_OUTPUT; // IMPORTANT set as output mode

    io_conf.pin_bit_mask = (1ULL << 2); // set as output pin 2
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    
    gpio_config(&io_conf);

    esp_timer_handle_t timer;
    esp_timer_create_args_t timer_args = {
        .callback = &blinkHandle,
        .name = "blinkTimer"
    };

    esp_timer_create(&timer_args, &timer);
    esp_timer_start_periodic(timer, TIMER_DELAY);
  
}
