/**
 * @file heater_task.c
 * @brief Heater control task based on the temperature and system state. 
 *
 * This task checks the system state and the current temperature, and controls
 * the heater GPIO output accordingly. The heater turns on if the system is in 
 * the on-state and the current temperature is more than 2 degrees below the 
 * setpoint.
 * 
 * The execution time of each read cycle is measured using `k_uptime_get()` and the
 * longest duration observed is tracked (excluding the first run).
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include "rtdb.h"

/**
 * @def OUTPUT_NODE
 * @brief DeviceTree alias for the heater output control pin.
 */
#define OUTPUT_NODE DT_ALIAS(outputpin)

/**
 * @brief GPIO specification for the heater control pin.
 */
static const struct gpio_dt_spec heater_pin = GPIO_DT_SPEC_GET(OUTPUT_NODE, gpios);

/**
 * @brief Task that turns the heater on/off based on system temperature and state.
 *
 * The heater is turned on if:
 * - The system is on.
 * - The current temperature is more than 2 degrees below the setpoint.
 *
 * It compares the setpoint and current temperature, updates the heater pin,
 * and logs/ prints any state changes.
 *
 * @param a Unused
 * @param b Unused
 * @param c Unused
 */
void heater_task(void *a, void *b, void *c) {
    
    gpio_pin_configure_dt(&heater_pin, GPIO_OUTPUT_INACTIVE);

    bool last_state = false;  
    int64_t max_duration = 0;
    bool first_run = true;

    while (1) {
        /* Before task starts */
        int64_t start_time = k_uptime_get();  // Start clock

        k_mutex_lock(&rtdb_mutex, K_FOREVER);

        bool heater_on = false;

        if (rtdb.system_on) 
        {
            int diff = rtdb.set_temp - rtdb.current_temp;
            heater_on = diff > 2;
        }

        if (heater_on != last_state) 
        {
        printk("Heater turned %s (set=%d, temp=%d)\n", heater_on ? "ON" : "OFF", rtdb.set_temp, rtdb.current_temp);
        last_state = heater_on;
        }

        gpio_pin_set_dt(&heater_pin, heater_on ? 1 : 0);
        k_mutex_unlock(&rtdb_mutex);

        /* After task is finished */
        int64_t end_time = k_uptime_get();  // End clock
        int64_t duration = end_time - start_time;

        if (!first_run && duration > max_duration) {
            max_duration = duration;
        }

        // printk("Cycle time(heater_task): %lld ms (Max: %lld ms)\n", duration, max_duration);

        first_run = false; 

        k_msleep(200);
    }
}

