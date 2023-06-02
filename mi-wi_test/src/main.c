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
#include <stdio.h>

#define MAX 50
uint16_t result;

uint16_t msg[10];
bool flag=0;
uint16_t count=0;
uint16_t voltage;
bool usart_flag;

struct adc_module adc_instance;
struct usart_module usart_instance;
struct tc_module tc_instance;

void usart_read_callback(struct usart_module *const module_inst);

void tc_callback_to_adc(struct tc_module *const module_inst);

int main(void)
{
	system_interrupt_enable_global();
	
	struct tc_config config_tc;
	struct port_config pin_conf;
	struct adc_config config_adc;
	struct usart_config config_usart;

	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
	config_tc.counter_8_bit.period =100;
	config_tc.count_direction =TC_COUNT_DIRECTION_UP;
	config_tc.reload_action=TC_RELOAD_ACTION_GCLK;

	tc_init(&tc_instance, TC1, &config_tc);
	tc_enable(&tc_instance);
	tc_register_callback(&tc_instance, tc_callback_to_adc, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);

	port_get_config_defaults(&pin_conf);
	pin_conf.direction = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(PIN_PA07, &pin_conf);

	adc_get_config_defaults(&config_adc);
	config_adc.clock_source= GCLK_GENERATOR_0;
	config_adc.reference = ADC_REFCTRL_REFSEL_INTVCC2;
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV4;
	config_adc.resolution= ADC_RESOLUTION_12BIT;
	config_adc.positive_input= ADC_POSITIVE_INPUT_PIN7;
	config_adc.negative_input= ADC_NEGATIVE_INPUT_GND;

	adc_init(&adc_instance, ADC, &config_adc);
	adc_enable(&adc_instance);

	usart_get_config_defaults(&config_usart);
	config_usart.baudrate= 9600;
	config_usart.mux_setting = CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad2 = CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = CDC_SERCOM_PINMUX_PAD3;

	usart_init(&usart_instance, CDC_MODULE, &config_usart);
	usart_enable(&usart_instance);

	while(1)
	{	
		adc_start_conversion(&adc_instance);	
		if(flag==1)
		{
			adc_read(&adc_instance, &result);
			voltage= ((result*3300)/4095)/0.3377;
			sprintf(msg, "%d\r\n", voltage);
			if(usart_flag=1)
			{
				usart_write_buffer_job(&usart_instance, msg, sizeof(msg));
			}
		}
	    flag=0;
}
}

void usart_read_callback(struct usart_module *const module_inst)
{
		usart_flag=1;	
}

void tc_callback_to_adc(struct tc_module *const module_inst)
{
	if(count==MAX)
	{
		flag=1;
		count=0;
	}
	else
	{
		count++;
	}
}


