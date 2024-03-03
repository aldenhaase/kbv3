#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#define KEYS_STACK_SIZE 2048
#define KEYS_PRIORITY   1

#define keys DT_NODELABEL(r0c0)

void init(void){
    const struct gpio_dt_spec keys = GPIO_DT_SPEC_GET(keys, gpios);
}

K_THREAD_DEFINE(keys_thread, KEYS_STACK_SIZE, init, NULL, NULL, NULL,
        KEYS_PRIORITY, K_ESSENTIAL, 0);
