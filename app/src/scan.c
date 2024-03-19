#include "scan.h"

#include <string.h>
#include <sys/_stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/class/hid.h>
#include <zephyr/usb/class/usb_hid.h>
#include <zephyr/usb/usb_device.h>

#define SCAN_STACK_SIZE 2048
#define SCAN_PRIORITY 5
#define SCAN_DEBOUNCE_COUNT 10

#define SLEEP_TIME_US 500
#define SCAN_MAX_SUBS 6

LOG_MODULE_REGISTER(scan, CONFIG_LOG_DEFAULT_LEVEL);

struct scan_sub_data_priv {
    struct scan_sub_data user_data;
    struct k_timer timer;
};

static struct scan_context{
    uint8_t debounce_states[SCAN_NUM_ROWS][SCAN_NUM_COLUMNS];
    struct scan_sub_data_priv subs[SCAN_MAX_SUBS];
    struct scan_report report;
}ctx;

#define LIST_OF_COLUMN_NAMES \
    X(c0)                    \
    X(c1)                    \
    X(c2)                    \
    X(c3)                    \
    X(c4)                    \
    X(c5)                    \
    X(c6)                    \
    X(c7)                    \
    X(c8)                    \
    X(c9)                    \
    X(c10)                   \
    X(c11)

#define LIST_OF_ROW_NAMES \
    X(r0)                 \
    X(r1)                 \
    X(r2)                 \
    X(r3)

#define X(name) GPIO_DT_SPEC_GET(DT_NODELABEL(name), gpios),
static const struct gpio_dt_spec rows[SCAN_NUM_ROWS] = {LIST_OF_ROW_NAMES};

static const struct gpio_dt_spec columns[SCAN_NUM_COLUMNS] = {
    LIST_OF_COLUMN_NAMES};
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
    for (int i = 0; i < SCAN_NUM_COLUMNS; ++i) {
        if (!gpio_is_ready_dt(&columns[i])) {
            __ASSERT_NO_MSG(false);
        }
        if (gpio_pin_configure_dt(&columns[i], GPIO_INPUT) != 0) {
            __ASSERT_NO_MSG(false);
        }
    }
}

static void configure_row_pins(void) {
    for (int i = 0; i < SCAN_NUM_ROWS; ++i) {
        if (!gpio_is_ready_dt(&rows[i])) {
            __ASSERT_NO_MSG(false);
        }
        if (gpio_pin_configure_dt(&rows[i], GPIO_OUTPUT) != 0) {
            __ASSERT_NO_MSG(false);
        }
    }
}

enum scan_error_codes Scan_Subscribe(struct scan_sub_data *sub_data) {
    for (int i = 0; i < SCAN_MAX_SUBS; ++i) {
        if (ctx.subs[i].user_data.receive_matrix_state != NULL) {
            ctx.subs[i].user_data = *sub_data;
            k_timer_init(&ctx.subs[i].timer, NULL, NULL);
            k_timer_start(&ctx.subs[i].timer, ctx.subs[i].user_data.alert_period,
                          ctx.subs[i].user_data.alert_period);
            return SCAN_ERR_OK;
        }
    }
    return SCAN_ERR_NO_MORE_SUBS;
}

static void alert_subs() {
    for (int i = 0; i < SCAN_MAX_SUBS; ++i) {
        if (ctx.subs[i].user_data.receive_matrix_state == NULL) {
            return;
        }
        if (k_timer_status_get(&ctx.subs[i].timer)) {
            ctx.subs[i].user_data.receive_matrix_state(ctx.report);
        }
    }
}

static void run(void) {
    while (1) {
        for (int i = 0; i < SCAN_NUM_ROWS; ++i) {
            activate_row(rows[i]);
            for (int j = 0; j < SCAN_NUM_COLUMNS; ++j) {
                if (read_column(columns[j])) {
                    if (ctx.debounce_states[i][j] == SCAN_DEBOUNCE_COUNT
                            ? true
                            : ++ctx.debounce_states[i][j] && false) {
                        ctx.report.matrix_state[i][j] = 1;
                    }
                }
            }
            deactivate_row(rows[i]);
        }
        alert_subs();
        memset(&ctx.report, 0, sizeof(ctx.report));
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

