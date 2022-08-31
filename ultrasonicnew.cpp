/*
 * ultrasonic.cpp
 *
 * Created: 01-06-2015 13:03:59
 *  Author: Urvashi
 */ 



#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
void lcd_init();
void disp_cmd(unsigned int cmd);
void disp_data(int cmd);
void disp_num(unsigned int num);
void disp_string(char *str);
int adc_read(int channel);
void trigger();
int running;
//volatile long avg = 0?
//volatile unsigned char up = 0?
//volatile uint32_t running = 0;
//volatile uint32_t timercounter =0?


 

        int main()
        {
	        
	        DDRD = 0XFF;
			DDRC = 0X01;
			PORTC = 0b00000010;
	        lcd_init();
	        disp_cmd(0X80);
	        disp_string("WELCOME");
			 
	       _delay_ms(1000);
	        disp_cmd(0X01);
	        _delay_ms(2);
	        disp_cmd(0X80);
	        disp_string("Enter the value");
			
		      trigger();
		
			
			int x,y, i=0;
			 
			
			/*x=adc_read(1);
			//_delay_ms(10);
			//disp_cmd(0X01);
			_delay_ms(2);
			disp_cmd(0XC5);
			disp_num(x);
			_delay_ms(1000);
			*/
			
			
		//adc_read(0)= 1;
			//if(running==0)
			//{
			
			if(PINC==0b00000000)	
			{
			
				if (PINC==0b00000010)
				{
					PORTB=0b00001111;
					disp_cmd(0X01);
					_delay_ms(2);
					disp_cmd(0X85);
					disp_num(i);
					
						/*disp_cmd(0X01);
						_delay_ms(10);
						y = (i*170);
						_delay_ms(2);
						disp_cmd(0XC5);
						disp_num(y);
						_delay_ms(1000);*/
					//break;
				}
				
				
			else {
				
				_delay_ms(1);
				i++;
			      }
			
			
			}
			
			
			}					
			
    





			

int adc_read(int channel)
{
	
	ADMUX=0X40 + channel;
	ADCSRA=0XD7;
	while((ADCSRA & 0X10) ==0);
	return ADC;
}
void disp_cmd(unsigned int cmd)
{
	PORTD=(cmd & 0XF0);
	PORTD=PORTD + 0X02;
	_delay_ms(2);
	PORTD=PORTD - 0X02;
	PORTD=((cmd<<4)& 0XF0);
	PORTD=PORTD + 0X02;
	_delay_ms(2);
	PORTD=PORTD - 0X02;
	
	
}
void disp_data( int cmd)
{
	PORTD=(cmd  & 0XF0);
	PORTD=PORTD + 0X03;
	_delay_ms(2);
	PORTD=PORTD - 0X03;
	PORTD=((cmd <<4)& 0XF0);
	PORTD=PORTD + 0X03;
	_delay_ms(2);
	PORTD=PORTD - 0X03;
}

void disp_string( char *str)
{
	int i=0;
	while (str[i]!='\0')
	{
		disp_data (str[i]);
		i++;
		
	}
}

void disp_num(unsigned int num)
{
	int a=0;
	if (num==0)
	{
		disp_data(48);
	}
	disp_cmd (0X04);
	while (num!=0)
	{
		a=num % 10;
		disp_data(a+48);
		num=num/10;
	}
	disp_cmd (0X06);
}
void lcd_init()
{
	disp_cmd(0X02);
	disp_cmd(0X28);
	disp_cmd(0X0C);
	disp_cmd(0X06);
}

void trigger()
{
	PORTC = 0X00; // clear to zero for 1 us
	_delay_us(1);
	PORTC = 0X01; // set high for 10us
	PORTB = 0XFF;
	running = 1; // sonar launched
	_delay_us(10);
	PORTC = 0X00; // clear
	PORTB = 0X00;
	running = 0;
}