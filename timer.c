/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * This is a bare minimum user application template.
 *
 * For documentation of the board, go \ref group_common_boards "here" for a link
 * to the board-specific documentation.
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# Basic usage of on-board LED and button
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include <delay.h>

struct tc_module tc_instance;

void tc_callback_to_toggle_led(struct tc_module *const module_inst);

int main (void)
{
	system_init();
	//sysclk_init();
	delay_init();
	board_init();
	// system_clock_init();
	
	
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
	config_tc.counter_8_bit.period = 5000;
	tc_init(&tc_instance, TC0, &config_tc);
	tc_enable(&tc_instance);
		
    tc_register_callback(&tc_instance, tc_callback_to_toggle_led, TC_CALLBACK_OVERFLOW);
	 tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);

// 	/* Insert application code here, after the board has been initialized. */
// 	//system_gclk_gen_enable(GCLK_GENERATOR_0);
// // 	tc_init(TC_CTRLA_MODE_COUNT16,TC1,GCLK_GENERATOR_0);
// // 	PORT_PIN_DIR_OUTPUT == SYSTEM_PINMUX_PIN_DIR_OUTPUT;
// 	//TC_COUNTER_SIZE_16BIT == TC_CTRLA_MODE_COUNT16;
// 	
// 	//TC_CLOCK_PRESCALER_DIV1 ==TC_CTRLA_PRESCALER(0);
// 	
// 	//tc_enable(TC_COUNTER_SIZE_16BIT);
// 	//TC_COUNT_DIRECTION_UP;
// 	//tc_start_counter(TC_COUNTER_SIZE_16BIT);
// 	//uint16_t tim_value=0;
// 	//tc_set_count_value(TC_COUNTER_SIZE_16BIT,tim_value);
// 	//tc_get_count_value(TC_COUNTER_SIZE_16BIT);
	
	//while (1) {
		
		//if (tc_get_count_value(TC_COUNTER_SIZE_16BIT) - tim_value>=18661)
		//{
			
			//	port_pin_toggle_output_level(LED1_PIN);
				//delay_ms(1000);
				//tc_get_count_value(TC_COUNTER_SIZE_16BIT);
				
				
	//	}

	//	}
		

	}
	
void tc_callback_to_toggle_led(struct tc_module *const module_inst)
{
	port_pin_toggle_output_level(LED1_PIN);
}	