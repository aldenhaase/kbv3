#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include "keypad_encoder.h"

LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

static void key_event_cb(struct keypad_event event) {
   LOG_INF("KEY EVENT!");
}

int main(void){
    const struct device *tca8418 = DEVICE_DT_GET(DT_NODELABEL(keypad_encoder));
    KEYPAD_ENCODER_SUBSCRIBE(tca8418, key_event_cb, KEYPAD_EVENT_KEY);
    return 0;
}
