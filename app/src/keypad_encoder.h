#include <sys/_stdint.h>
#include <zephyr/device.h>

enum KEYPAD_EVENT_TYPE {
    KEYPAD_EVENT_KEY      = 1<<0,
    KEYPAD_EVENT_MODIFIER = 1<<1,
    KEYPAD_EVENT_KBV3     = 1<<2,
};

struct keypad_event {
    uint8_t event_code;
    enum KEYPAD_EVENT_TYPE type;
};

struct keypad_encoder_api {
    void (*subscribe_to_key_events)( void (*cb)(const struct keypad_event),
            uint8_t sub_flags);
};

ALWAYS_INLINE static void KEYPAD_ENCODER_SUBSCRIBE(const struct device *dev,
        void (*cb)(const struct keypad_event), enum KEYPAD_EVENT_TYPE sub_flags)
{
    const struct keypad_encoder_api *api =
        (const struct keypad_encoder_api *)dev->api;
    return api->subscribe_to_key_events(cb, sub_flags);
}

