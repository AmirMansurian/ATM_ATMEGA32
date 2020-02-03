/*
 * ATM.c
 *
 * Created: 2/2/2020 12:58:49 AM
 *  Author: Amir
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>


#define MAX_USER 5
#define NAME_SIZE 17
#define ID_SIZE 15
#define PASS_SIZE 5
#define UART_PORT PORTD
#define UART_DDR DDRD
#define UART_T PD1
#define UART_R PD0
#define ctrl PORTB
#define en PB2
#define rw PB1
#define rs PB0
#define KEY_PORT PORTC
#define KEY_DDR DDRC




unsigned char col, row;//key pressed detection
unsigned char key;//key detected from keypad
unsigned char display [17];
unsigned char uart_read [15];
unsigned char password [5];
unsigned int i=0, ret;


void lcd_cmd (unsigned char command);
void init_lcd(void);
void usartinit(unsigned char *str);
void lcd_data(unsigned char data);
void lcd_write_string();
void lcd_loc (unsigned char x, unsigned char y);
void debouncing ();


unsigned int user_detection ();
unsigned int Authentication (unsigned cahr idno);
void show_menu ();


unsigned char keypad[4][4]={ '1', '2', '3', 'A',
							 '4', '5', '6', 'B',
							 '7', '8', '9', 'C',
							 '*', '0', '#', 'D' };

unsigned char users [MAX_USER][ID_SIZE];
unsigned char pass [MAX_USER][PASS_SIZE];
unsigned char name [MAX_USER][NAME_SIZE];
int money [MAX_USER];



int main(void)
{
	DDRA=0xff;
	DDRB=0xff;
	KEY_DDR=0xf0;
	UART_DDR=0x00;
	KEY_PORT=0xff;
	
	init_lcd();
	_delay_ms(50);
	usartinit();
	
	while (1)
	{
		lcd_cmd(0x01);
		strcpy(display, "Insert your card");
		lcd_write_string(display);
		
		while ((UCSRA) & (1<<RXC))
		{
			uart_read[i] = UDR;
			_delay_ms(1);
			i++;
			
			if (i==12)
			{
				uart_read[i]=0;
				break;
			}
			
		}
		
		if (!ret=user_detection())
			continue;
		
		show_menu();	
	
	
	
	
	
	}
	
	
	
	
	
	
	

	return 0;	
}





void lcd_cmd (unsigned char command)
{
    PORTA=command;
    PORTB &= ~(1<<RS);
    PORTB|= (1<<E);
    _delay_ms(50);
    PORTB &= ~(1<<E);
    PORTA =0;
}

void lcd_data (unsigned char data)
{
 PORTB=data;
 ctrl=(1<<rs)|(0<<rw)|(1<<en);
 _delay_ms(1);
 ctrl=(1<<rs)|(0<<rw)|(0<<en);
 _delay_ms(50);
 return;
}

void lcd_write_string(unsigned char *str)         //LCD string write sub routine
{
 	unsigned char i = 0;
	
	while (str[i]!=0)
	{
		lcd_data(str[i]);
		i++;
	}
 return;
}

void usartinit()                       //USART initialization
{
 UBRRH=00;
 UBRRL=77;
 UCSRB|=(1<<RXEN);
 UCSRC|=(1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
}

void debouncing ()
{
	do 
		{
			PORTC &= 0x0f;
			col= (PINC & 0x0f);
			
		} while (col !=0x0f);
			
		do 
		{
			do 
			{
				_delay_ms(20);
				col = (PINC & 0x0f);
				
			} while (col == 0x0f);
			
			_delay_ms(20);
			col = (PINC & 0x0f);
			
		} while (col == 0x0f);
		
		while (1)
		{
			PORTC = 0xef;
			col = (PINC & 0x0f);
			
			if (col != 0x0f)
			{
				row=0;
				break;
			}
			
			PORTC = 0xdf;
			col = (PINC & 0x0f);
			
			if (col !=0x0f)
			{
				row=1;
				break;
			}
			
			PORTC = 0xbf;
			col = (PINC & 0x0f);
			
			if (col != 0x0f)
			{
				row=2;
				break;
			}
			
			PORTC = 0x7f;
			col = (PINC & 0x0f);
			if (col != 0x0f)
			{
				row=3;
				break;
			}
		}
		
		
		if (col == 0x0e)
			key = keypad[row][2];
		else if (col == 0x0d)
			key = keypad[row][1];
		else if (col == 0x0b)
			key = keypad[row][0];
}

void init_lcd ()                     //LCD initialization
{
	_delay_ms(50);
    send_a_command(0x01);// sending all clear command
    send_a_command(0x38);// 16*2 line LCD
    send_a_command(0x0E);// screen and cursor ON
	
 return;
}

unsigned int user_detection()
{	
	unsigned char i=0;
	
	lcd_cmd(0x01);
	
	while (i<MAX_USER)
	{
		if (!strcmp(users[i], uart_read))
		{
			strcpy (display, "Hi ");
			lcd_write_string(display);
			strcpy(display, name[i]);
			lcd_write_string(display);
			_delay_ms(2000);
			
			
			lcd_cmd(0x01);
			strcpy (display, "Enter password:");
			lcd_write_string(display);
			lcd_loc(1, 2);
			
			return Authentication(i);
			
		}
					
		i++;
	}
	
	strcpy (display, "Invalid card !!!");
	lcd_write_string(display);
	_delay_ms(2000);
			
	return 0;	
}

void show_menu()
{
	lcd_cmd(0x01);
	
	strcpy (display, "1- Inventory");
	lcd_write_string(display);
	lcd_loc(1, 2);
	strcpy (display, "2- Withdraw");
	lcd_write_string(display);
	_delay_ms(3000);
	
	lcd_cmd(0x01);
	
	strcpy (display, "3- Change pass");
	lcd_write_string(display);
	lcd_loc(1, 2);
	strcpy (display, "4- Transfer");
	lcd_write_string(display);
	_delay_ms(3000);	
}

unsigned int Authentication (unsigned char idno)
{
	unsigned char j=0, try=0;
	
	while (try<3)
	{
		j=0;
			
		while (j<4)
			{
				do 
				{
					PORTC &= 0x0f;
					col= (PINC & 0x0f);
			
				} while (col !=0x0f);
			
				do 
				{
					do 
					{
						_delay_ms(20);
						col = (PINC & 0x0f);
				
					} while (col == 0x0f);
			
					_delay_ms(20);
					col = (PINC & 0x0f);
			
				} while (col == 0x0f);
		
				while (1)
				{
					PORTC = 0xef;
					col = (PINC & 0x0f);
			
					if (col != 0x0f)
					{
						row=0;
						break;
					}
			
					PORTC = 0xdf;
					col = (PINC & 0x0f);
			
					if (col !=0x0f)
					{
						row=1;
						break;
					}
			
					PORTC = 0xbf;
					col = (PINC & 0x0f);
			
					if (col != 0x0f)
					{
						row=2;
						break;
					}
			
					PORTC = 0x7f;
					col = (PINC & 0x0f);
					if (col != 0x0f)
					{
						row=3;
						break;
					}
				}
				
					if (col == 0x0e)
						password[j] =keypad[row][0];
					else if (col == 0x0d)
						password[j] = keypad[row][1];
					else if (col == 0x0b)
						password[j] = keypad[row][2];
					else 
						password[j] = keypad[row][3]; 
					
				lcd_data('*');				
				j++;
		
			}
			
			_delay_ms(500);
			
			if (!strcmp(password, pass[idno]))
				return 1;
				
			lcd_cmd(0x01);
			lcd_loc(1, 1);
			strcpy (display, "Invalid pass:");
			lcd_write_string(display);
			lcd_loc(1, 2);
			
				
		try++;
	}
	
	lcd_cmd(0x01);
	lcd_loc(1, 1);
	
	return 0;
}


