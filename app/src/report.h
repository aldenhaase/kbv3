#include "zephyr/usb/class/hid.h"
#include <sys/_stdint.h>

#define REPORT_MAX_KEYS_PRESSED 6

void REPORT_APPEND_KEYS(enum hid_kbd_code* keys);
