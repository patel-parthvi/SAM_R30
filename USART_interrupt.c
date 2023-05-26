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
//#include<delay.h>

struct usart_module usart_instance;
#define MAX_RX_BUFFER_LENGTH   5
#define max_val 100

struct tc_module tc_instance;


struct usart_module usart_instance;

int i;
//volatile uint8_t rx_buffer;
volatile uint8_t tx_buffer[MAX_RX_BUFFER_LENGTH ];
volatile uint8_t rx_buffer;
void tc_callback_to_count(struct tc_module *const tc_module);
void usart_read_callback(struct usart_module *const usart_module);
//void usart_write_callback(struct usart_module *const usart_module);
int flag;
int count=0;
uint16_t tim;

uint8_t timer_flag = 0;

int main()
{
	system_interrupt_enable_global();

	uint8_t string[] = "Hello World!\r\n";

 	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
 	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
 	config_tc.clock_source = GCLK_GENERATOR_0;
 	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
 	config_tc.counter_8_bit.period = 10;
 	tc_init(&tc_instance, TC0, &config_tc);
 	tc_enable(&tc_instance);
 
 	tc_register_callback(&tc_instance, tc_callback_to_count, TC_CALLBACK_OVERFLOW);
 	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);

	
    struct usart_config config_usart;
    usart_get_config_defaults(&config_usart);

    config_usart.baudrate    = 9600;
    config_usart.mux_setting = CDC_SERCOM_MUX_SETTING;
   
    config_usart.pinmux_pad2 = CDC_SERCOM_PINMUX_PAD2;
    config_usart.pinmux_pad3 = CDC_SERCOM_PINMUX_PAD3;
	
	usart_init(&usart_instance, CDC_MODULE, &config_usart) ;
	usart_enable(&usart_instance);
	
 	//usart_register_callback(&usart_instance, usart_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
 	usart_register_callback(&usart_instance, usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
 	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
     usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_RECEIVED);
	
    	 
    while (1)
	{ 
 	if(usart_write_buffer_job(&usart_instance,(uint8_t*) tx_buffer, MAX_RX_BUFFER_LENGTH)==STATUS_OK);
{

usart_read_buffer_job(&usart_instance, rx_buffer, MAX_RX_BUFFER_LENGTH);

usart_read_buffer_wait(&usart_instance, rx_buffer, MAX_RX_BUFFER_LENGTH);
}


		/*if(usart_read_buffer_wait(&usart_instance, rx_buffer,2))
		{
			flag = 1;
		
		
			}*/
	
		}
			
			
    
  	/* if( usart_write_buffer_job(&usart_instance, (uint8_t*)tx_buffer, MAX_RX_BUFFER_LENGTH)==STATUS_OK)
 		 
     	usart_read_buffer_job(&usart_instance, rx_buffer, MAX_RX_BUFFER_LENGTH);  
 
 		tx_buffer[MAX_RX_BUFFER_LENGTH]++;
 		usart_read_buffer_wait(&usart_instance, (uint8_t*)tx_buffer, MAX_RX_BUFFER_LENGTH); 	 
     }*/
}

 void usart_read_callback(struct usart_module *const usart_module)
{
usart_write_buffer_job(&usart_instance, (uint8_t*)tx_buffer, MAX_RX_BUFFER_LENGTH);
}
// 
// void usart_read_callback(struct usart_module *const usart_module)
// {
// 	
//   usart_write_buffer_job(&usart_instance, (uint8_t*)tx_buffer, MAX_RX_BUFFER_LENGTH);
//   
// 	//rx_buffer=tx_buffer[MAX_RX_BUFFER_LENGTH]++;
//   
// }



 void tc_callback_to_count(struct tc_module *const tc_module)
 {
	 uint8_t string[] = "Hello World!\r\n";
	if(count==max_val)
{
usart_write_buffer_wait(&usart_instance,string, sizeof(string));
count = 0;
}
else
{
count++;
}
}