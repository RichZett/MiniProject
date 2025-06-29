/**
 * @file main.c
 * @brief Entry point of the application that initializes GPIOs and starts system tasks.
 *
 * This file sets up the LED and button GPIOs and launches separate threads for
 * handling buttons, LEDs, temperature sensing, and heater control.
 */

#include <zephyr/kernel.h>          /* for k_msleep() */
#include <zephyr/device.h>          /* for device_is_ready() and device structure */
#include <zephyr/devicetree.h>		/* for DT_NODELABEL() */
#include <zephyr/drivers/gpio.h>    /* for GPIO api */
#include <stdlib.h>  				/* for abs() */
#include "rtdb.h"


/** @brief The polling time for the loop in milliseconds. */
#define SLEEP_TIME_MS 100 

/** @brief DeviceTree aliases for LEDs. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

/** @brief DeviceTree aliases for buttons. */
#define BUT0_NODE DT_ALIAS(sw0)
#define BUT1_NODE DT_ALIAS(sw1)
#define BUT2_NODE DT_ALIAS(sw3) 

/** @brief GPIO specifications for LEDs. */
static const struct gpio_dt_spec led 		= GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_normal = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led_low  	= GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led_high 	= GPIO_DT_SPEC_GET(LED3_NODE, gpios);

/** @brief GPIO specifications for buttons. */
static const struct gpio_dt_spec but 	 = GPIO_DT_SPEC_GET(BUT0_NODE, gpios);
static const struct gpio_dt_spec but_inc = GPIO_DT_SPEC_GET(BUT1_NODE, gpios);
static const struct gpio_dt_spec but_dec = GPIO_DT_SPEC_GET(BUT2_NODE, gpios);

/** @brief Stack and thread data for button task. */
K_THREAD_STACK_DEFINE(button_stack, 512);
struct k_thread button_thread_data;
extern void button_task(void *, void *, void *);

/** @brief Stack and thread data for LED task. */
K_THREAD_STACK_DEFINE(led_stack, 512);
struct k_thread led_thread_data;
extern void led_task(void *, void *, void *);

/** @brief Stack and thread data for temperature task. */
K_THREAD_STACK_DEFINE(temp_stack, 512);
struct k_thread temp_thread_data;
extern void temp_task(void *, void *, void *);

/** @brief Stack and thread data for heater task. */
K_THREAD_STACK_DEFINE(heater_stack, 512);
struct k_thread heater_thread_data;
extern void heater_task(void *, void *, void *);

/**
 * @brief Main function/ entry point of the system.
 *
 * The main function initializes all the needed GPIO devices for input/ output.
 * It checks if they are ready and creates threads for handling button input,
 * temperature sensing, LED control, and heater control.
 *
 * @return Always returns 0.
 */
int main(void)
{
	int ret =0;
	
	/* Check if devices are ready */
	const struct device *gpio0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
	if (!device_is_ready(gpio0)) 
	{
		return 0;
	}
	
	/* 	Configure the GPIO pins for input/output and set active logic */
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}
	ret = gpio_pin_configure_dt(&led_normal, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}
	ret = gpio_pin_configure_dt(&led_low, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}
	ret = gpio_pin_configure_dt(&led_high, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}
	ret = gpio_pin_configure_dt(&but, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}
	ret = gpio_pin_configure_dt(&but_inc, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&but_dec, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	/* Create the threads */
	k_thread_create(&button_thread_data, button_stack, 512, button_task, NULL, NULL, NULL, 5, 0, K_NO_WAIT);
	k_thread_create(&led_thread_data, led_stack, 512, led_task, NULL, NULL, NULL, 5, 0, K_NO_WAIT);
	k_thread_create(&temp_thread_data, temp_stack, 512, temp_task, NULL, NULL, NULL, 1, 0, K_NO_WAIT);
	k_thread_create(&heater_thread_data, heater_stack, 512, heater_task, NULL, NULL, NULL, 3, 0, K_NO_WAIT);

	while(1) {
		k_msleep(100);  /* idle loop */  
	}

	return 0;
}