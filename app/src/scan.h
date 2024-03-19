#pragma once

#include "zephyr/sys_clock.h"
#define SCAN_NUM_ROWS 4
#define SCAN_NUM_COLUMNS 12

#include <sys/_stdint.h>
#include <zephyr/kernel.h>

struct scan_report {
    uint8_t matrix_state[SCAN_NUM_ROWS][SCAN_NUM_COLUMNS];
};

struct scan_sub_data {
    void(*receive_matrix_state)(const struct scan_report);
    k_timeout_t alert_period;
};

enum scan_error_codes {
    SCAN_ERR_NO_MORE_SUBS = -1,
    SCAN_ERR_OK = 0,
};

enum scan_error_codes Scan_Subscribe(struct scan_sub_data *sub_data);
