#include "zephyr/dt-bindings/gpio/gpio.h"
#include <sys/_stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#define KEYS_STACK_SIZE 2048
#define KEYS_PRIORITY   6

#define keys DT_NODELABEL(r0c0)

static struct gpio_callback hello_data;

void print_hello(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    printk("HELLO?");
}

void init(){
    const struct gpio_dt_spec keys = GPIO_DT_SPEC_GET(keys, gpios);
    gpio_pin_configure(keys.port, keys.pin, GPIO_PULL_UP | GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&keys, GPIO_INT_EDGE_FALLING);
    gpio_init_callback(&hello_data, print_hello, BIT(keys.pin));
    gpio_add_callback(keys.port, &hello_data);
}

K_THREAD_DEFINE(keys_thread, KEYS_STACK_SIZE, init, NULL, NULL, NULL,
        KEYS_PRIORITY, 0, 0);
