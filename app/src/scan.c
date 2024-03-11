#include <sys/_stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/class/usb_hid.h>
#include <zephyr/usb/class/hid.h>

#define SCAN_STACK_SIZE 2048
#define SCAN_PRIORITY   5

#define SLEEP_TIME_US 500
#define NUM_ROWS 4
#define NUM_COLUMNS 12

LOG_MODULE_REGISTER(scan, CONFIG_LOG_DEFAULT_LEVEL);

extern enum hid_kbd_code key_mapping[4][12];

static const uint8_t hid_kbd_report_desc[] = HID_KEYBOARD_REPORT_DESC();
static const struct device * kb_dev;
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

static void configure_hid(void) {
	kb_dev = device_get_binding("HID_0");
    usb_hid_register_device(kb_dev, hid_kbd_report_desc,
            sizeof(hid_kbd_report_desc), NULL);
    usb_hid_init(kb_dev);
    usb_enable(NULL);
}

struct kb_report {
    union {
        uint8_t bytes[8];
        struct {
            uint8_t modifier;
            uint8_t reserved;
            uint8_t keypress[6];
        } rep_fields;
    };
    uint8_t size;
    uint8_t len;
};

static void run(void) {
    configure_column_pins();
    configure_row_pins();
    configure_hid();
    while(1) {
        k_msleep(1);
    }
	while (1) {
        struct kb_report kb_report = {.size = 6};
        for (int i = 0; i < NUM_ROWS; ++i) {
            activate_row(rows[i]);
            for (int j = 0; j < NUM_COLUMNS; ++j) {
                if (read_column(columns[j]) == 1) {
                    if(kb_report.size == kb_report.len) {
                        LOG_ERR("No more room in report");
                    }
                    kb_report.rep_fields.keypress[kb_report.len++] =
                        key_mapping[i][j];
                }
            }
            deactivate_row(rows[i]);
        }
        if(kb_report.len) {
            hid_int_ep_write(kb_dev, kb_report.bytes, sizeof(kb_report.bytes), NULL);
        }
        k_usleep(SLEEP_TIME_US);
	}
}

K_THREAD_DEFINE(scan_thread, SCAN_STACK_SIZE, run, NULL, NULL, NULL,
        SCAN_PRIORITY, K_ESSENTIAL, 0);

