/**
 * @file temp_task.c
 * @brief Temperature sensing task using a TC74 I2C temperature sensor.
 *
 * This task periodically reads the temperature from a TC74 sensor over I2C
 * and updates the shared system state in the RTDB (Real-Time Database).
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include "rtdb.h"

/**
 * @def TC74_NODE
 * @brief DeviceTree node label for the TC74 temperature sensor.
 */

/**
 * @def TC74_CMD_RTR
 * @brief Command to read the temperature register from the TC74 sensor.
 */
#define TC74_NODE DT_NODELABEL(tc74sensor)
static const struct i2c_dt_spec tc74 = I2C_DT_SPEC_GET(TC74_NODE);
#define TC74_CMD_RTR 0x00  

/**
 * @brief Task reads TC74 sensor to get the temperature and updates RTDB.
 *
 * This task connects the I2C interface to the TC74 sensor. When the bus is ready,
 * it reads the current temperature every second.
 * 
 * Prints an error message if the I2C reading fails. 
 *
 * @param a Unused
 * @param b Unused
 * @param c Unused
 */
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
