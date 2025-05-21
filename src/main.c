#include <zephyr/kernel.h>          /* for k_msleep() */
#include <zephyr/device.h>          /* for device_is_ready() and device structure */
#include <zephyr/devicetree.h>		/* for DT_NODELABEL() */
#include <zephyr/drivers/gpio.h>    /* for GPIO api*/

/* Define the polling time, in ms */
#define SLEEP_TIME_MS 100

/* Get led0 and button0 node IDs. Refer to the DTS file */
#define LED0_NODE DT_ALIAS(led0)
#define BUT0_NODE DT_ALIAS(sw0)

/* Get the device pointer, pin number, and configuration flags for led0 and button 0. A build error on this line means your board is unsupported. */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec but = GPIO_DT_SPEC_GET(BUT0_NODE, gpios);

/*
 * The main function
 */
int main(void)
{
	int ret =0;
	
	/* Check if device are ready */
	if (!device_is_ready(led.port) || !device_is_ready(but.port)) {
		return 0;
	}	

	/* Configure the GPIO pins for input/output and set active logic */
	/* Note that the devicetree activates the internal pullup and sets the active low flag */
	/*   so an external resistor is not needed and pressing the button causes a logic level of 1*/
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&but, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	/*
	 * The main loop
	 */
	bool last_butStat = false;
	bool system_on = false;

	while(1) {
		bool current_butStat = gpio_pin_get_dt(&but);

		// Detect rising edge (button pressed)
		if (current_butStat && !last_butStat) {
			system_on = !system_on;
			printk("System toggled %s\n", system_on ? "ON" : "OFF");

			// Set LED accordingly
			gpio_pin_set_dt(&led, system_on ? 1 : 0);

			k_msleep(300); // debounce delay
		}

		last_butStat = current_butStat;
		k_msleep(SLEEP_TIME_MS);
	}

	return 0;
}