/**
 * @file led_task.c
 * @brief LED logic implementation of project
 *
 * This task uses four LEDs to visually represent the status of the system
 * including power state and temperature deviation.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <stdlib.h>
#include "rtdb.h"

/**
 * @def LED0_NODE
 * @brief DeviceTree alias for LED0 - indicates system ON/OFF state.
 */

/**
 * @def LED1_NODE
 * @brief DeviceTree alias for LED1 - indicates the temperature is within the normal range.
 */

/**
 * @def LED2_NODE
 * @brief DeviceTree alias for LED2 - indicates the temperature is too low.
 */

/**
 * @def LED3_NODE
 * @brief DeviceTree alias for LED3 - indicates the temperature is too high.
 */
/* LED alias */
#define LED0_NODE DT_ALIAS(led0)    /* ON/OFF */
#define LED1_NODE DT_ALIAS(led1)    /* Normal range */
#define LED2_NODE DT_ALIAS(led2)    /* Too low */
#define LED3_NODE DT_ALIAS(led3)    /* Too high */

/**
 * @brief GPIO specification for each LED.
 */
static const struct gpio_dt_spec led        = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_normal = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led_low    = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led_high   = GPIO_DT_SPEC_GET(LED3_NODE, gpios);


/**
 * @brief Task that updates the LED states based on system status.
 *
 * This task runs continuously, checking the system state from the RTDB. 
 * It communicates with the user by altering the four LEDs as follows: 
 * - LED0: ON if system is on.
 * - LED1: ON if temperature difference is within the normal range, i.e. ±2°C.
 * - LED2: ON if temperature is 2°C (or more) below the setpoint.
 * - LED3: ON if temperature is 2°C (or more) above the setpoint.
 *
 * @param a Unused
 * @param b Unused
 * @param c Unused
 */
void led_task(void *a, void *b, void *c) {
    gpio_pin_configure_dt(&led,        GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_normal, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_low,    GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_high,   GPIO_OUTPUT_INACTIVE);

    while (1) {
        k_mutex_lock(&rtdb_mutex, K_FOREVER);

        /* LED0: system ON/OFF indicator */
        gpio_pin_set_dt(&led, rtdb.system_on ? 1 : 0);

        if (rtdb.system_on) {
            int diff = rtdb.current_temp - rtdb.set_temp;

            gpio_pin_set_dt(&led_normal, abs(diff) <= 2 ? 1 : 0);
            gpio_pin_set_dt(&led_low,    diff < -2 ? 1 : 0);
            gpio_pin_set_dt(&led_high,   diff > 2 ? 1 : 0);
        } else {
            gpio_pin_set_dt(&led_normal, 0);
            gpio_pin_set_dt(&led_low,    0);
            gpio_pin_set_dt(&led_high,   0);
        }

        k_mutex_unlock(&rtdb_mutex);
        k_msleep(200);
    }
}
