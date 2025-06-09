/**
 * @file button_task.c
 * @brief Button input control for the system. 
 * 
 * This task reads the button control input of three buttons, and changes the state of the system accordingly.
 * It can toggle ON/OFF the system and increase/decrease the temperature setpoint. 
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include "rtdb.h"

/**
 * @def BUT0_NODE
 * @brief DeviceTree alias for button 0 - toggles the system ON/OFF.
 */

/**
 * @def BUT1_NODE
 * @brief DeviceTree alias for button 1 - increases the temperature setpoint.
 */

/**
 * @def BUT2_NODE
 * @brief DeviceTree alias for button 2 - decreases the temperature setpoint.
 */
#define BUT0_NODE DT_ALIAS(sw2)
#define BUT1_NODE DT_ALIAS(sw1)
#define BUT2_NODE DT_ALIAS(sw3)

/**
 * @brief GPIO specifications for the buttons.
 */
static const struct gpio_dt_spec but      = GPIO_DT_SPEC_GET(BUT0_NODE, gpios);
static const struct gpio_dt_spec but_inc  = GPIO_DT_SPEC_GET(BUT1_NODE, gpios);
static const struct gpio_dt_spec but_dec  = GPIO_DT_SPEC_GET(BUT2_NODE, gpios);

/**
 * @brief The task monitors the button input and updates state of the system. 
 *
 * This task runs continuously, reads the input from three buttons as follows:
 * - Button 0 is used to toggle the system ON/ OFF.
 * - Button 1 is used to increase the temperature setpoint.
 * - Button 2 is used to decrease the temperature setpoint.
 *
 * @param a Unused
 * @param b Unused
 * @param c Unused
 */
void button_task(void *a, void *b, void *c) {
    
    bool last_on = false, last_inc = false, last_dec = false;

    gpio_pin_configure_dt(&but, GPIO_INPUT);
    gpio_pin_configure_dt(&but_inc, GPIO_INPUT);
    gpio_pin_configure_dt(&but_dec, GPIO_INPUT);


    while (1) {

        bool on  = gpio_pin_get_dt(&but);
        bool inc = gpio_pin_get_dt(&but_inc);
        bool dec = gpio_pin_get_dt(&but_dec);

        k_mutex_lock(&rtdb_mutex, K_FOREVER);
        if (on && !last_on) {
            rtdb.system_on = !rtdb.system_on;
            printk("System toggled %s\n", rtdb.system_on ? "ON" : "OFF");
        }
        if (inc && !last_inc) {
            rtdb.set_temp++;
            printk("Setpoint increased to %d\n", rtdb.set_temp);
        }
        if (dec && !last_dec) {
            rtdb.set_temp--;
            printk("Setpoint decreased to %d\n", rtdb.set_temp);
        }
        k_mutex_unlock(&rtdb_mutex);

        last_on = on;
        last_inc = inc;
        last_dec = dec;

        k_msleep(200);
    }
}
