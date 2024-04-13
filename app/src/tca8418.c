#include <sys/_stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/__assert.h>

#include "keypad_encoder.h"
#include "tca8418_i2c.h"
#include "tca8418_regs.h"

#define DT_DRV_COMPAT ti_tca8418

#define TCA8418_MAX_SUBSCRIBERS 8

LOG_MODULE_REGISTER(tca8418, CONFIG_LOG_DEFAULT_LEVEL);

struct tca8418_config {
    uint16_t addr_7b;
};

struct tca8418_data {
    const struct device *i2c;
    const struct gpio_dt_spec gpio_spec;
    const struct device *self;
};

struct tca8418_subscriber {
    void (*cb)(struct keypad_event);
    uint8_t flags;
};

static struct tca8418_ctx {
    struct tca8418_subscriber subs[TCA8418_MAX_SUBSCRIBERS];
    const struct tca8418_data *ddata;
    const struct tca8418_config *config;
} tca8418_ctx;

static void subscribe_to_key_event(void (*cb)(struct keypad_event),
                                   uint8_t flags) {
    for (int i = 0; i < TCA8418_MAX_SUBSCRIBERS; ++i) {
        if (tca8418_ctx.subs[i].cb == NULL) {
            tca8418_ctx.subs[i].cb = cb;
            tca8418_ctx.subs[i].flags = flags;
            return;
        }
    }
    __ASSERT(false, "Too many subscribers for tca8418");
}

__attribute__((
    unused)) /**< Put here to avoid compiler warning when device is disabled */
static const struct keypad_encoder_api keypad_encoder_api = {
    .subscribe_to_key_events = subscribe_to_key_event,
};

void tca8418_alert_subs_handler(struct k_work *work) {
    uint8_t tca8418_event;
    tca8418_i2c_get_event(tca8418_ctx.ddata->i2c, tca8418_ctx.config->addr_7b,
                          &tca8418_event);
    tca8418_i2c_clear_intr(tca8418_ctx.ddata->i2c, tca8418_ctx.config->addr_7b);
    struct keypad_event event = {.event_code = tca8418_event};
    for (int i = 0; i < TCA8418_MAX_SUBSCRIBERS; ++i) {
        if (tca8418_ctx.subs[i].cb == NULL) {
            return;
        }
        tca8418_ctx.subs[i].cb(event);
    }
}

K_WORK_DEFINE(tca8418_alert_subs_work, tca8418_alert_subs_handler);

static struct gpio_callback tca8418_cb_data;

static void tca8418_data_ready_cb(const struct device *tca8418,
                                  struct gpio_callback *cb, uint32_t pins) {
    k_work_submit(&tca8418_alert_subs_work);
}

static int tca8418_init(const struct device *dev) {
    tca8418_ctx.ddata = (const struct tca8418_data *)(dev)->data;
    tca8418_ctx.config = (const struct tca8418_config *)(dev)->config;

    const struct gpio_dt_spec intr = tca8418_ctx.ddata->gpio_spec;
    if (!gpio_is_ready_dt(&intr)) {
        __ASSERT(false, "Intr gpio not ready");
    }
    if (gpio_pin_configure_dt(&intr, GPIO_INPUT) != 0) {
        __ASSERT(false, "Failed to configure gpio as input");
    }
    if (gpio_pin_interrupt_configure_dt(&intr, GPIO_INT_EDGE_TO_ACTIVE)) {
        __ASSERT(false, "Failed to configure gpio intr");
    }
    gpio_init_callback(&tca8418_cb_data, tca8418_data_ready_cb, BIT(intr.pin));
    gpio_add_callback(intr.port, &tca8418_cb_data);

    if (!device_is_ready(tca8418_ctx.ddata->i2c)) {
        __ASSERT(false, "i2c device not ready");
    }

    const uint8_t tca8418_config =
        TCA8418_CFG_SET_KE_IEN_BIT | TCA8418_CFG_SET_INT_CFG_BIT;
    tca8418_i2c_write_config(tca8418_ctx.ddata->i2c,
                             tca8418_ctx.config->addr_7b, tca8418_config);

    return 0;
};

#define TCA8418_INIT(inst)                                                    \
    static struct tca8418_data tca8418_data##inst = {                         \
        .i2c = DEVICE_DT_GET(DT_INST_BUS(inst)),                              \
        .gpio_spec = GPIO_DT_SPEC_GET(DT_INST(inst, ti_tca8418), gpios),      \
        .self = DEVICE_DT_GET(DT_INST(inst, ti_tca8418)),                     \
    };                                                                        \
    static const struct tca8418_config tca8418_config##inst = {               \
        .addr_7b = DT_INST_REG_ADDR(inst),                                    \
    };                                                                        \
    DEVICE_DT_INST_DEFINE(                                                    \
        inst, tca8418_init, NULL, &tca8418_data##inst, &tca8418_config##inst, \
        POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &keypad_encoder_api);

DT_INST_FOREACH_STATUS_OKAY(TCA8418_INIT)
