#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BLINK_DELAY_MS 1000


void app_main(void)
{
    gpio_config_t io_conf ;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE; // IMPORTANTE: desable interruptions
    io_conf.mode = GPIO_MODE_OUTPUT; // IMPORTANT set as output mode

    io_conf.pin_bit_mask = (1ULL << 2); // set as output pin 2
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    
    
    gpio_config(&io_conf);


    while (1) {
        gpio_set_level(GPIO_NUM_2, 1);
        vTaskDelay(BLINK_DELAY_MS / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_2, 0);
        vTaskDelay(BLINK_DELAY_MS / portTICK_PERIOD_MS);
    }

}

