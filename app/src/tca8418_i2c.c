#include <sys/_stdint.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/__assert.h>

#include "tca8418_regs.h"
#include "zephyr/logging/log.h"

LOG_MODULE_REGISTER(tca8418_i2c, CONFIG_LOG_DEFAULT_LEVEL);

void tca8418_i2c_get_event(const struct device *i2c_dev, uint16_t addr_7b,
                           uint8_t *out_buff) {
    if (i2c_reg_read_byte(i2c_dev, addr_7b, TCA8418_KEY_EVENT_REG, out_buff) !=
        0) {
        LOG_ERR("Failed to read from tca8418");
    }
}

void tca8418_i2c_clear_intr(const struct device *i2c_dev, uint16_t addr_7b) {
    const uint8_t data[2] = {TCA8418_INT_STAT_REG,
                             TCA8418_INT_STAT_K_INT_CLEAR};
    if (i2c_write(i2c_dev, data, 2, addr_7b) != 0) {
        LOG_ERR("Failed to clear tca8418 intr");
    }
}
void tca8418_i2c_write_config(const struct device *i2c_dev, uint16_t addr_7b,
                              const uint8_t config) {
    const uint8_t data[2] = {TCA8418_CFG_REG, config};
    if (i2c_write(i2c_dev, data, 2, addr_7b) != 0) {
        LOG_ERR("Failed to write to tca8418 config reg");
    }
}
