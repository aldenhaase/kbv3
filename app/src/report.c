#include <string.h>
#include <sys/_stdint.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>
#include <zephyr/usb/class/hid.h>

#include "report.h"
#include "zephyr/logging/log.h"

LOG_MODULE_REGISTER(report, CONFIG_LOG_DEFAULT_LEVEL);

#define REPORT_STACK_SIZE 2048
#define REPORT_PRIORITY   5
#define REPORT_MSGQ_MAX_ITEMS 8
#define REPORT_MSGQ_SIZE REPORT_MSGQ_MAX_ITEMS * sizeof(uint8_t)

K_MSGQ_DEFINE(report_kb_q, REPORT_MSGQ_SIZE, REPORT_MSGQ_MAX_ITEMS, 2);

static const uint8_t hid_kbd_report_desc[] = HID_KEYBOARD_REPORT_DESC();
static const struct device * kb_dev;

static struct report_kb_data {
    union {
        uint8_t bytes[8];
        struct {
            uint8_t modifier;
            uint8_t reserved;
            uint8_t keypress[REPORT_MAX_KEYS_PRESSED];
        } rep_fields;
    };
} kb_data;

void REPORT_APPEND_KEYS(enum hid_kbd_code *keys) {
    memcpy(kb_data.rep_fields.keypress, keys, REPORT_MAX_KEYS_PRESSED);
    k_msgq_put(&report_kb_q, kb_data.bytes, K_NO_WAIT);
}

static void configure_hid(void) {
	kb_dev = device_get_binding("HID_0");
    usb_hid_register_device(kb_dev, hid_kbd_report_desc,
            sizeof(hid_kbd_report_desc), NULL);
    usb_hid_init(kb_dev);
    usb_enable(NULL);
}

static void run(void) {
    while(1) {
        uint8_t bytes[8];
        k_msgq_get(&report_kb_q, &bytes, K_FOREVER);
        hid_int_ep_write(kb_dev, bytes, sizeof(bytes), NULL);
    }
}

static void init() {
    configure_hid();
    run();
}

K_THREAD_DEFINE(report_thread, REPORT_STACK_SIZE, init, NULL, NULL, NULL,
        REPORT_PRIORITY, K_ESSENTIAL, 0);
