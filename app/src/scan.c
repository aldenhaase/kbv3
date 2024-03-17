#include <sys/_stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/class/usb_hid.h>
#include <zephyr/usb/class/hid.h>

#include "report.h"
#include "key_mapping.h"

#define SCAN_STACK_SIZE 2048
#define SCAN_PRIORITY   5
#define SCAN_DEBOUNCE_COUNT 10

#define SLEEP_TIME_US 500
#define NUM_ROWS 4
#define NUM_COLUMNS 12

LOG_MODULE_REGISTER(scan, CONFIG_LOG_DEFAULT_LEVEL);

#define LIST_OF_COLUMN_NAMES\
    X(c0)                   \
    X(c1)                   \
    X(c2)                   \
    X(c3)                   \
    X(c4)                   \
    X(c5)                   \
    X(c6)                   \
    X(c7)                   \
    X(c8)                   \
    X(c9)                   \
    X(c10)                  \
    X(c11)                  \

#define LIST_OF_ROW_NAMES   \
    X(r0)                   \
    X(r1)                   \
    X(r2)                   \
    X(r3)                   \

#define X(name) GPIO_DT_SPEC_GET(DT_NODELABEL(name), gpios),
static const struct gpio_dt_spec rows[NUM_ROWS] = {
    LIST_OF_ROW_NAMES
};

static const struct gpio_dt_spec columns[NUM_COLUMNS] = {
    LIST_OF_COLUMN_NAMES
};
#undef X

static int read_column(const struct gpio_dt_spec column_dev) {
    return gpio_pin_get(column_dev.port, column_dev.pin);
}

static void activate_row(const struct gpio_dt_spec row_dev) {
   gpio_pin_set(row_dev.port, row_dev.pin, 1);
}


static void deactivate_row(const struct gpio_dt_spec row_dev) {
   gpio_pin_set(row_dev.port, row_dev.pin, 0);
}

static void configure_column_pins(void) {
    for(int i = 0; i < NUM_COLUMNS; ++i) {
        if (!gpio_is_ready_dt(&columns[i])) {
            __ASSERT_NO_MSG(false);
        }
        if(gpio_pin_configure_dt(&columns[i], GPIO_INPUT) != 0) {
            __ASSERT_NO_MSG(false);
        }
    }
}

static void configure_row_pins(void) {
    for(int i = 0; i < NUM_ROWS; ++i) {
        if (!gpio_is_ready_dt(&rows[i])) {
            __ASSERT_NO_MSG(false);
        }
        if(gpio_pin_configure_dt(&rows[i], GPIO_OUTPUT) != 0) {
            __ASSERT_NO_MSG(false);
        }
    }
}

static uint8_t debounce_states[NUM_ROWS][NUM_COLUMNS];
static enum hid_kbd_code prev_high_pins[REPORT_MAX_KEYS_PRESSED];

static bool has_state_changed(enum hid_kbd_code *curr) {
    for(int i = 0; i < REPORT_MAX_KEYS_PRESSED; ++i) {
        if(prev_high_pins[i] != curr[i]) {
            return true;
        }
    }
    return false;
}

static void run(void) {
	while (1) {
        enum hid_kbd_code high_pins[REPORT_MAX_KEYS_PRESSED] = {0};
        uint8_t num_high_pins = 0;
        for (int i = 0; i < NUM_ROWS; ++i) {
            activate_row(rows[i]);
            for (int j = 0; j < NUM_COLUMNS; ++j) {
                if(read_column(columns[j])) {
                    if(num_high_pins < REPORT_MAX_KEYS_PRESSED) {
                        if(debounce_states[i][j] == SCAN_DEBOUNCE_COUNT ? true :
                                ++debounce_states[i][j] && false){
                            high_pins[num_high_pins++] = key_mapping[i][j];
                        }
                    }else {
                        debounce_states[i][j]>0?--debounce_states[i][j]:0xDEAD;
                    }
                }
            }
            deactivate_row(rows[i]);
        }
        if(has_state_changed(high_pins)) {
            REPORT_APPEND_KEYS(high_pins);
            memcpy(prev_high_pins, high_pins, REPORT_MAX_KEYS_PRESSED);
        }
        k_usleep(SLEEP_TIME_US);
	}
}

static void init() {
    configure_column_pins();
    configure_row_pins();
    run();
}

K_THREAD_DEFINE(scan_thread, SCAN_STACK_SIZE, init, NULL, NULL, NULL,
        SCAN_PRIORITY, K_ESSENTIAL, 0);

