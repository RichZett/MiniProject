
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include "rtdb.h"

// TC74 I2C configuration (from devicetree overlay)
#define TC74_NODE DT_NODELABEL(tc74sensor)
static const struct i2c_dt_spec tc74 = I2C_DT_SPEC_GET(TC74_NODE);
#define TC74_CMD_RTR 0x00  // Temperature register

void temp_task(void *a, void *b, void *c) {
    if (!device_is_ready(tc74.bus)) {
        printk("I2C bus not ready\n");
        return;
    }

    uint8_t cmd = TC74_CMD_RTR;
    uint8_t temp_raw;

    while (1) {
        int ret = i2c_write_read_dt(&tc74, &cmd, 1, &temp_raw, 1);
        if (ret == 0) {
            int temp = (int8_t)temp_raw;  // Sign-extend to handle negative temperatures

            k_mutex_lock(&rtdb_mutex, K_FOREVER); 
            rtdb.current_temp = temp;
            k_mutex_unlock(&rtdb_mutex);

            printk("Current temp: %d\n", temp);
        } else {
            printk("Failed to read temp sensor (code %d)\n", ret);
        }

        k_msleep(1000);
    }
}
