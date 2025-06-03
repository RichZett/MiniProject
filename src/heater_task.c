
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include "rtdb.h"

/* Define the GPIO pin for the heater */
#define HEATER_NODE DT_ALIAS(heater)
static const struct gpio_dt_spec heater = {.port = DEVICE_DT_GET(DT_NODELABEL(gpio0)), .pin = 17,.dt_flags = GPIO_ACTIVE_HIGH, };

void heater_task(void *a, void *b, void *c) {
    gpio_pin_configure_dt(&heater, GPIO_OUTPUT_INACTIVE);

    bool last_state = false;  

    while (1) {
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

        gpio_pin_set_dt(&heater, heater_on ? 1 : 0);
        k_mutex_unlock(&rtdb_mutex);

        k_msleep(200);
    }
}
