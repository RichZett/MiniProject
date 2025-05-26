#include <zephyr/kernel.h>          /* for k_msleep() */
#include <zephyr/device.h>          /* for device_is_ready() and device structure */
#include <zephyr/devicetree.h>		/* for DT_NODELABEL() */
#include <zephyr/drivers/gpio.h>    /* for GPIO api */
#include <stdlib.h>  				/* for abs() */


/* Define the polling time, in ms */
#define SLEEP_TIME_MS 100 // Remove??

/* Get leds and buttons node IDs. Refer to the DTS file */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#define BUT0_NODE DT_ALIAS(sw0)
#define BUT1_NODE DT_ALIAS(sw1)
#define BUT2_NODE DT_ALIAS(sw3) 

/* Get the device pointer, pin number, and configuration flags for led0 and button 0. A build error on this line means your board is unsupported. */
/* LEDs*/
static const struct gpio_dt_spec led 		= GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_normal = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led_low  	= GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led_high 	= GPIO_DT_SPEC_GET(LED3_NODE, gpios);

/* Buttons */
static const struct gpio_dt_spec but 	 = GPIO_DT_SPEC_GET(BUT0_NODE, gpios);
static const struct gpio_dt_spec but_inc = GPIO_DT_SPEC_GET(BUT1_NODE, gpios);
static const struct gpio_dt_spec but_dec = GPIO_DT_SPEC_GET(BUT2_NODE, gpios);
 
/*
 * The main function
 */
int main(void)
{
	int ret =0;
	
	/* Check if devices are ready */
	if (!device_is_ready(led.port) || !device_is_ready(but.port)) {
		return 0;
	}	
	if (!device_is_ready(but_inc.port)) {
		return 0;
	}
	if (!device_is_ready(but_dec.port)) {
		return 0;
	}
	
	/* Configure the GPIO pins for input/output and set active logic */
	/* Note that the devicetree activates the internal pullup and sets the active low flag */
	/*   so an external resistor is not needed and pressing the button causes a logic level of 1*/
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

	/*
	 * The main loop
	 */
	bool last_butStat = false;
	bool last_butInc = false;
	bool last_butDec = false;

	bool system_on = false;
	int set_temp = 25; 
	int current_temp = 25;  // simulated temp


	while(1) {
		bool current_butStat = gpio_pin_get_dt(&but);
		bool current_butInc = gpio_pin_get_dt(&but_inc);
		bool current_butDec = gpio_pin_get_dt(&but_dec);


		/* ON/OFF with button1(sw0) */
		if (current_butStat && !last_butStat) {
			system_on = !system_on;
			printk("System toggled %s\n", system_on ? "ON" : "OFF");

			// Set LED accordingly
			gpio_pin_set_dt(&led, system_on ? 1 : 0); 

			k_msleep(300); // debounce delay
		}

		/* Increase the set_temp with button2(sw1) */
		if (current_butInc && !last_butInc)
		{
			set_temp++; 
			printk("The set temp increased to %d\n", set_temp);
			k_msleep(300); // debounce

		}

		/* Decrease the set_temp with button4(sw3)*/
		if (current_butDec && !last_butDec) 
		{
		set_temp--;
		printk("The set temp decreased to %d\n", set_temp);
		k_msleep(300);
		}

		last_butInc = current_butInc;
		last_butDec = current_butDec;
		last_butStat = current_butStat;
		
		if (system_on) {
			int diff = current_temp - set_temp;

			gpio_pin_set_dt(&led_normal, (abs(diff) <= 2) ? 1 : 0);
			gpio_pin_set_dt(&led_low,  (diff < -2) ? 1 : 0);
			gpio_pin_set_dt(&led_high, (diff > 2) ? 1 : 0);
		} else {
			// All off if system is off
			gpio_pin_set_dt(&led_normal, 0);
			gpio_pin_set_dt(&led_low,  0);
			gpio_pin_set_dt(&led_high, 0);
		}
 
		k_msleep(SLEEP_TIME_MS); 
	}

	return 0;
}