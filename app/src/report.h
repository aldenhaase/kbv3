#pragma once

#include <sys/_stdint.h>

#include "scan.h"

#define REPORT_MAX_KEYS_PRESSED 6

void REPORT_APPEND_KEYS(struct scan_high_pin* high_pins, uint8_t num_pins);
