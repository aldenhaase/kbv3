#include <zephyr/device.h>

void tca8418_i2c_get_event(const struct device *i2c_dev, uint16_t addr_7b,
                      uint8_t *out_buff);

void tca8418_i2c_clear_intr(const struct device *i2c_dev, uint16_t addr_7b);

void tca8418_i2c_write_config(const struct device *i2c_dev, uint16_t addr_7b,
                         const uint8_t config);
