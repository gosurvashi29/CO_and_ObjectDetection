/*
 * ultrasonic.cpp
 *
 * Created: 01-06-2015 13:03:59
 *  Author: Urvashi
 */ 



#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#define BR 9600UL
#define PS ((F_CPU/(16*BR))-1)

void lcd_init();
void disp_cmd(unsigned int cmd);
void disp_data(int cmd);
void disp_num(unsigned int num);
void disp_string(char *str);
int adc_read(int channel);
void trigger();
void trigger2();
float servo(int x,float y);
void ultrasonic();
void ultrasonic2();
void co();
int  a=0;
uint16_t r; 


 
int adc_read(int channel)
{
	
	ADMUX=0X40 + channel;
	ADCSRA=0XD7;
	while((ADCSRA & 0X10) ==0);
	return ADC;
}

uint16_t getPulseWidth()
{
	uint32_t i,result;
	for(i=0;i<600000;i++)
	{
		if(!(PINC & (1<<PC1))) continue; else break;
	}

	if(i==600000)
		return 0xffff; //Indicates time out

	TCCR1A=0X00;
	TCCR1B=(1<<CS11); //Prescaler = Fcpu/8
	TCNT1=0x00; //Init counter

	for(i=0; i<=600000; i++)
	{
		if(PINC & (1<<PC1))
		{
			if(TCNT1 > 60000) break; else continue;
		}
		else
			break;
	}

	if(i==600000)
		return 0xffff; //Indicates time out

	result=TCNT1;

	TCCR1B=0x00;
	if(result > 60000)
		return 0xfffe; //No obstacle
	else
		//return (result>>1);
		return result;
}
uint16_t getPulseWidth2()
{
	uint32_t i,result;
	for(i=0;i<600000;i++)
	{
		if(!(PINC & (1<<PC3))) continue; else break;
	}

	if(i==600000)
	return 0xffff; //Indicates time out

	TCCR1A=0X00;
	TCCR1B=(1<<CS11); //Prescaler = Fcpu/8
	TCNT1=0x00; //Init counter

	for(i=0; i<=600000; i++)
	{
		if(PINC & (1<<PC3))
		{
			if(TCNT1 > 60000) break; else continue;
		}
		else
		break;
	}

	if(i==600000)
	return 0xffff; //Indicates time out

	result=TCNT1;

	TCCR1B=0x00;
	if(result > 60000)
	return 0xfffe; //No obstacle
	else
	//return (result>>1);
	return result;
}
void usart_init()
{
	UCSRB =(1<<RXEN) | (1<<TXEN);
	UCSRC =(1<<URSEL) | (1<<USBS) | (1<<UCSZ1) | (1<<UCSZ0);
	UBRRL=PS;
	UBRRH=(PS>>8);
}

void usart_tx(unsigned char data)
{
	while((UCSRA & 0x20)==0);
	UDR=data;
}

unsigned int usart_rx()
{
	while((UCSRA & 0x80)==0);
	return UDR;
}
void usart_txstring(char *str)
{
	int i=0;
	while (str[i]!='\0')
	{
		usart_tx(str[i]);
		_delay_ms(100);
		i++;
		
	}
}
int main()
{	
	DDRC=0XD5;// co sensor on pc5 and ultrasonic trigger1 on pc0,trig2 0n pc2,echo1 on pc1,echo2 on pc3
	DDRD=0XFF;//LCD
	DDRB=0X1E;//servo ON PB1, MOTOR1 ON PB3 and PB4
	//DDRB=0X1E// MOTORS ON PORTB 1234
	
	int j;
	float z=1;
	
	while(1)
	{
		char c= usart_rx();
	
		
		

		if(c=='F')
		{
			PORTB=0b00001010;
			usart_txstring("forward");
			_delay_ms(100);
			
		}

		else if(c=='B')
		{
			PORTB=0b00010100;
			usart_txstring("backward");
			_delay_ms(100);
		}

		else if(c=='R')
		{
			PORTB=0b00000001;
			usart_txstring("right");
			_delay_ms(100);
		}

		else if(c=='L')
		{
			PORTB=0b00000010;
			usart_txstring("left");
			_delay_ms(100);
		}

		else if(c=='S')
		{
			PORTB=0b00000000;
			usart_txstring("stop");
			_delay_ms(100);
		}
		
		
		ultrasonic();
		ultrasonic2();
		
		j=z*10;
		
		if (z==1.5)
		{
			if (a==0)
			{
			
			a=1;
			}			
			else if (a==1)
			{
				a=0;
			}
		}
		z = servo(j,z);
		co();
		
	}
}

/*void disp_cmd(unsigned int cmd)
{
	PORTD=(cmd & 0XF0);
	PORTD=PORTD + 0X02;
	_delay_ms(1);
	PORTD=PORTD - 0X02;
	PORTD=((cmd<<4)& 0XF0);
	PORTD=PORTD + 0X02;
	_delay_ms(1);
	PORTD=PORTD - 0X02;
}
void disp_data( int cmd)
{
	PORTD=(cmd  & 0XF0);
	PORTD=PORTD + 0X03;
	_delay_ms(1);
	PORTD=PORTD - 0X03;
	PORTD=((cmd <<4)& 0XF0);
	PORTD=PORTD + 0X03;
	_delay_ms(1);
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
	disp_cmd(0x01);
}*/

void trigger()
{	
	PORTC=0x00;
	_delay_us(10);
	PORTC=0x01;
	_delay_us(15);
	PORTC=0x00;
	_delay_us(20);
}
void trigger2()
{
	PORTC=0x00;
	_delay_us(10);
	PORTC=0x04;
	_delay_us(15);
	PORTC=0x00;
	_delay_us(20);
}
float servo(int j,float z)
{
	TCCR1A=0;
	ICR1=19999;
	
	TCCR1A = (1<< WGM11);
	TCCR1B = (1<< WGM13) | (1<< WGM12) | (1 << CS11);
	
	DDRB=0XFF;
	
	TCCR1A |= 2<<6;
	TCCR1A |= 2<<4;
switch (j)
{
case 10 :  OCR1A = ICR1 * (z/20);
	      _delay_ms(100);
		   z=z+0.5;
		   break;
		   //return z;
case 15:  OCR1A = ICR1 * (z/20);
	     _delay_ms(100);
		 if (a==0)
		 { 
			  z=z-0.5;
		 }
		 if (a==1)
		  {
			  z=z+0.5;
		  }
		  break;
		  //return z;
case 20:   OCR1A = ICR1 * (z/20);
		    _delay_ms(100);
	    	z=z-0.5;
			break;
		    //return z;


}

}

void ultrasonic()
{
	
	trigger();
	r=getPulseWidth();
	
	if(r==0xffff)
	{
		usart_txstring("Error        ");
		usart_tx(0X0D);
		
	}
	else if(r==0xfffe)
	{
		usart_txstring("Time out        ");
		usart_tx(0X0D);
		
	}
	else
	{
		int d;
		d=(r/58.0);
		usart_txstring("          cm  ");
		usart_tx(d);
		usart_tx(0X0D);
		
	}
}	

void ultrasonic2()
{
	
	trigger2();
	r=getPulseWidth2();
	
	if(r==0xffff)
	{
		usart_txstring("Error        ");
		usart_tx(0X0D);
		
	}
	else if(r==0xfffe)
	{
	   usart_txstring("Time out        ");
	   usart_tx(0X0D);
		
	}
	else
	{
		int d;
		d=(r/58.0);
		usart_txstring("          cm  ");
		usart_tx(d);
		usart_tx(0X0D);
		
	}
}

void co()
{
	int p;
	
	/*if (p> && p<)
	{
	
	disp_string("CO DETECTED");
	_delay_ms(100);
	}	*/
	
	p=adc_read(5);
	usart_tx(p);
	usart_tx(0X0D);
	
}