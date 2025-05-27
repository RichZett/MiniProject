
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <stdlib.h>
#include "rtdb.h"

/* LED alias */
#define LED0_NODE DT_ALIAS(led0)    /* ON/OFF */
#define LED1_NODE DT_ALIAS(led1)    /* Normal range */
#define LED2_NODE DT_ALIAS(led2)    /* Too low */
#define LED3_NODE DT_ALIAS(led3)    /* Too high */

/* GPIO specifications */
static const struct gpio_dt_spec led        = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_normal = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led_low    = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led_high   = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

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
