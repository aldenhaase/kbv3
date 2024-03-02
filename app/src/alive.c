#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#define ALIVE_STACK_SIZE 500
#define ALIVE_PRIORITY   5

#define SLEEP_TIME_MS   1500

#define LED0_NODE DT_ALIAS(led0)

LOG_MODULE_REGISTER(alive, CONFIG_LOG_DEFAULT_LEVEL);

static int configure(const struct gpio_dt_spec *led) {
	if (!gpio_is_ready_dt(led)) {
		return -1;
	}

	if (!gpio_pin_configure_dt(led, GPIO_OUTPUT_ACTIVE)){
		return -1;
    }
    return 0;
}

static void alive(void) {

    const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
    configure(&led);
	bool led_state = true;
	while (1) {
		__ASSERT(gpio_pin_toggle_dt(&led) == 0, "FAILED TO TOGGLE LED");
		led_state = !led_state;
		k_msleep(SLEEP_TIME_MS);
	}
}

K_THREAD_DEFINE(alive_thread, ALIVE_STACK_SIZE, alive, NULL, NULL, NULL,
        ALIVE_PRIORITY, K_ESSENTIAL, 0);
