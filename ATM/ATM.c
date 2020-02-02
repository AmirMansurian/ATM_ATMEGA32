/*
 * ATM.c
 *
 * Created: 2/2/2020 12:58:49 AM
 *  Author: Amir
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>


#define MAX_USER 10
#define ID_SIZE 15
#define PASS_SIZE 4


void lcd_cmd(unsigned char cmd);
void init_lcd(void);
void usartinit();
void lcd_write(unsigned char data);
void lcd_write_string();
void lcd_loc (unsigned char x, unsigned char y);

unsigned char keypad[4][3]={ '3', '2', '1',
							 '6', '5', '4',
							 '9', '8', '7',
							 '*', '0', '#' }

unsigned char users [MAX_USER][ID_SIZE];
unsigned char pass [PASS_SIZE];



int main(void)
{
	
}








void lcd_cmd(unsigned char cmd)            //LCD command subroutine
{
 PORTB=cmd;
 ctrl=(0<<rs)|(0<<rw)|(1<<en);
 _delay_ms(1);
 ctrl=(0<<rs)|(0<<rw)|(0<<en);
 _delay_ms(50);
 return;
}

void lcd_write(unsigned char data)
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
 int b=0;
 while(str[b]!='')
 {
  lcd_write(str[b]);
  b++;
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