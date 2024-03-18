#include <sys/_stdint.h>
#include <zephyr/usb/class/hid.h>

#include "scan.h"
#include "key_mapping.h"

#define KBV3_LAYER_SHIFT HID_KBD_MODIFIER_NONE

struct key_mapping_item_internal {
    struct key_mapping_item l0;
    struct key_mapping_item l1;
};

static uint8_t current_layer = 0;

static struct key_mapping_item_internal key_mapping[4][12] = {
    [0][0]   = {.l0.hid_code = HID_KBD_MODIFIER_LEFT_CTRL,
                .l0.type = KEY_MAPPING_MODIFIER_CODE,
                .l1.hid_code = HID_KBD_MODIFIER_LEFT_CTRL,
                .l1.type = KEY_MAPPING_MODIFIER_CODE},
    [0][1]   = {.l0.hid_code = HID_KEY_CAPSLOCK,
                .l0.type = KEY_MAPPING_KEY_CODE,
                .l1.hid_code = HID_KEY_CAPSLOCK,
                .l1.type = KEY_MAPPING_KEY_CODE},
    [0][2]   = {.l0.hid_code = HID_KBD_MODIFIER_LEFT_ALT,
                .l0.type = KEY_MAPPING_MODIFIER_CODE,
                .l1.hid_code = HID_KBD_MODIFIER_LEFT_ALT,
                .l1.type = KEY_MAPPING_MODIFIER_CODE},
    [0][3]   = {.l0.hid_code = KBV3_LAYER_SHIFT,
                .l0.type = KEY_MAPPING_KBV3_CODE,
                .l1.hid_code = KBV3_LAYER_SHIFT, //This should be impossible to press
                .l1.type = KEY_MAPPING_KBV3_CODE},
};

struct key_mapping_item KEY_MAPPING_GET_KEY_CODE(struct scan_high_pin high_pin) {
    if(current_layer == 0){
        return key_mapping[high_pin.c][high_pin.r].l0;
    }
    return key_mapping[high_pin.c][high_pin.r].l1;
}
