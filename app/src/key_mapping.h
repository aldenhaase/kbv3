#pragma once

#include <zephyr/usb/class/hid.h>
#include "scan.h"

enum key_mapping_type {
    KEY_MAPPING_KEY_CODE,
    KEY_MAPPING_MODIFIER_CODE,
    KEY_MAPPING_KBV3_CODE,
};

struct key_mapping_item {
    int hid_code;
    enum key_mapping_type type;
};

struct key_mapping_item KEY_MAPPING_GET_KEY_CODE(struct scan_high_pin high_pin);
