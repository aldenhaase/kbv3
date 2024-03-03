/*
# SPDX-License-Identifier: Apache-2.0
 *
 * vim: set ts=4 sw=4 softtabstop=4 expandtab colorcolumn=80:
 *
 * @file Extention to the Zephyr owned include/dt-bindings/gpio/gpio.h file
 *
 */

#pragma once

#define GPIO_INPUT              (1U << 16)
#define GPIO_OUTPUT             (1U << 17)
#define GPIO_DISCONNECTED	     0
#define GPIO_OUTPUT_INIT_LOW    (1U << 18)
#define GPIO_OUTPUT_INIT_HIGH   (1U << 19)
#define GPIO_ANALOG             (1U << 7)
#define GPIO_OUTPUT_LOW         (GPIO_OUTPUT | GPIO_OUTPUT_INIT_LOW)
#define GPIO_OUTPUT_HIGH        (GPIO_OUTPUT | GPIO_OUTPUT_INIT_HIGH)
#define GPIO_INT_ENABLE         (1U << 22)
#define GPIO_INT_EDGE           (1U << 24)
#define GPIO_INT_LOW_0          (1U << 25)
#define GPIO_INT_HIGH_1         (1U << 26)

#define GPIO_INT_EDGE_RISING           (GPIO_INT_ENABLE | \
					GPIO_INT_EDGE | \
					GPIO_INT_HIGH_1)

#define GPIO_INT_EDGE_FALLING          (GPIO_INT_ENABLE | \
					GPIO_INT_EDGE | \
					GPIO_INT_LOW_0)

#define GPIO_INT_EDGE_BOTH             (GPIO_INT_ENABLE | \
					GPIO_INT_EDGE | \
					GPIO_INT_LOW_0 | \
					GPIO_INT_HIGH_1)
