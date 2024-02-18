/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   100

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
	int ret;
	bool led_state = true;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return 0;
		}

		led_state = !led_state;
		//printf("LED state: %s\n", led_state ? "HELLO" : "WORLD");
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}

static int cmd_echo(const struct shell *shell, size_t argc, char *argv[]) {
    if(argc != 2) {
        shell_fprintf(shell, SHELL_WARNING, "ERROR MUST PROVIDE WORD\r\n");
        return -1;
    }
    shell_fprintf(shell, SHELL_NORMAL, "The Word Is %s\r\n", argv[1]);
    return 0;
}

//SHELL_STATIC_SUBCMD_SET_CREATE(
//    kbv3_cmds,
//    SHELL_CMD_ARG(echo, NULL,
//        "Echo\n"
//        "usage:\n"
//        "$ echo <word>\n"
//        "example:\n"
//        "$ echo hello\n",
//        cmd_echo, 1, 0),
//    SHELL_SUBCMD_SET_END
//    );

SHELL_CMD_REGISTER(echo, NULL, "ECHOS THE TEXT", cmd_echo);
