#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "bim_uart2.h"
#include "bim_icu.h"
#include "bim_gpio.h"
#if(UART2_DRIVER)
#define AON_GPIO16      *((volatile unsigned long *) (0x00800A00+0xe*4))
#define AON_GPIO17      *((volatile unsigned long *) (0x00800A00+0xf*4))
#define AON_GPIO0      *((volatile unsigned long *) (0x00800A00+0x0*4))
#define AON_GPIO1      *((volatile unsigned long *) (0x00800A00+0x1*4))
uint8_t uart2_tx_buf[UART2_FIFO_MAX_COUNT];
//==============================UART_INITIAL===================
void uart2_init(uint32_t baudrate)
{
    uint32_t    uart_clk_div;
    SET_UART2_POWER_UP ;   //open periph
    uart_clk_div = 16000000/baudrate - 1;
    AON_GPIO16 = (0x01 << 4) | (0x01 << 5)|(0x01 << 6); //second_func pull up
    AON_GPIO17 = (0x01 << 4) | (0x01 << 5)|(0x01 << 6); //second_func pull up
    UART2_REG0X0 = (uart_clk_div << POS_UART2_REG0X0_CLK_DIVID) |
                   (0x0          << POS_UART2_REG0X0_STOP_LEN ) |
                   (0x0          << POS_UART2_REG0X0_PAR_MODE ) |
                   (0x1          << POS_UART2_REG0X0_PAR_EN   ) |
                   (0x3          << POS_UART2_REG0X0_LEN      ) |
                   (0x0          << POS_UART2_REG0X0_IRDA     ) |
                   (0x1          << POS_UART2_REG0X0_RX_ENABLE) |
                   (0x1          << POS_UART2_REG0X0_TX_ENABLE) ;
    UART2_REG0X1 = 0x00004010;
    UART2_REG0X4 = 0x42;
    UART2_REG0X6 = 0x0;
    UART2_REG0X7 = 0x0;
    SYS_REG0X10_INT_EN |= (0x01 << POS_SYS_REG0X10_INT_EN_UART1);
}

void uart0_init(uint32_t baudrate)
{
    uint32_t    uart_clk_div;
    SET_UART0_POWER_UP ;   //open periph
    uart_clk_div = 16000000/baudrate - 1;
    AON_GPIO0 = (0x01 << 4) | (0x01 << 5)|(0x01 << 6); //second_func pull up
    AON_GPIO1 = (0x01 << 4) | (0x01 << 5)|(0x01 << 6); //second_func pull up
    UART0_REG0X0 = (uart_clk_div << POS_UART2_REG0X0_CLK_DIVID) |
                   (0x0          << POS_UART2_REG0X0_STOP_LEN ) |
                   (0x0          << POS_UART2_REG0X0_PAR_MODE ) |
                   (0x1          << POS_UART2_REG0X0_PAR_EN   ) |
                   (0x3          << POS_UART2_REG0X0_LEN      ) |
                   (0x0          << POS_UART2_REG0X0_IRDA     ) |
                   (0x1          << POS_UART2_REG0X0_RX_ENABLE) |
                   (0x1          << POS_UART2_REG0X0_TX_ENABLE) ;
    UART0_REG0X1 = 0x00004010;
    UART0_REG0X4 = 0x42;
    UART0_REG0X6 = 0x0;
    UART0_REG0X7 = 0x0;
    SYS_REG0X10_INT_EN |= (0x01 << POS_SYS_REG0X10_INT_EN_UART0);
}

static void uart_send_byte(unsigned char data)
{
    while( 0==(UART0_REG0X2&(1<<POS_UART2_REG0X2_FIFO_WR_READY)) )
    {
        ;
    }
    UART0_REG0X3 = data;
}



#if 1

void printf_string(char *strings)
{
	while(*strings != '\0')
	{
		uart_send_byte(*strings);
		strings ++;
	}
}
 
void printf_hex(uint32_t input_data)
{
	uint8_t temp[96];
	uint8_t i = 0;
    
	while(input_data > 0)
	{
        if((input_data % 16) > 9)
            temp[i]=(input_data % 16) - 10 + 'a';
        else
            temp[i]=(input_data % 16) + '0';   
		
		input_data = input_data / 16;
		i ++;
	}
    
	for( ; i>0; i --)
	{
		uart_send_byte(temp[i-1]);
	}
}
int bim_printf(const char *fmt,...)
{
	char chr;
	int argint;
    char temp;

	char *argstr;
	va_list va;				
	va_start(va,fmt);		
	
	while((chr= *fmt++) != '\0')
	{
		if(chr != '%')
		{
			uart_send_byte(chr);
			continue;
		}
        if(*fmt != '\0')
            temp=*fmt++;
        else
            temp='\0';
		
		switch(temp)
		{
			case '\0':
                uart_send_byte('%');
                break;
			case 'c':
				uart_send_byte(va_arg(va,int));
                break;
			case 's':
				argstr = va_arg(va,char *);
				printf_string(argstr);				
			    break;
			case 'x':
			case 'X':
				argint = va_arg(va,int);
				printf_hex(argint);
		        break;
			default :		
				uart_send_byte('%');
				fmt --;
				break;
			
		}
	}
	va_end(va);		
    return 0;
}
 


#endif
//==========================UART_INTERRUPT==========================
#else
void uart2_init(uint32_t baudrate)
{
    return ;
}
int bim_printf(const char *fmt,...)
{
    return 0;
}
#endif
