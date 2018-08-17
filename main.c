/*

	Arkanoid
	* 
	* R.Groener
	* 
	* Offene Punkte:
	* 
	* -Block seitendetktierung funktioniert noch nicht
	* -
	* -
	* 

*/

#include <avr/io.h>
#include "glcd/glcd.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "glcd/fonts/Liberation_Sans15x21_Numbers.h"
#include "glcd/fonts/font5x7.h"
#include <avr/pgmspace.h>
#include <util/delay.h>



//#define F_CPU 8000000UL  // 1 MHz

#define T_RED !(PIND & (1<<PD5)) && (entprell == 0)
#define T_BLUE !(PIND & (1<<PD6)) && (entprell == 0)
#define T_GREEN !(PIND & (1<<PD2)) && (entprell == 0)
#define RELOAD_ENTPRELL 2

#define LED_EIN PORTC |= (1<<PC3)
#define LED_AUS	PORTC &= ~(1<<PC3);					//LED ausschalten


#define UP 1
#define DOWN 0
#define LEFT 0
#define RIGHT 1

#define EIN 1
#define AUS 0

#define BLOCK_BOTTOM (by==(blocky+block_hight+ball_radius))&&((bx>(blockx-ball_radius))&&(bx<blockx+block_lenght+ball_radius))//funktioniert
#define BLOCK_TOP (by==(blocky-ball_radius))&&((bx>(blockx-ball_radius))&&(bx<blockx+block_lenght+ball_radius))//funktioniert
#define BLOCK_SIDE (((by+ball_radius)>=blocky)&&((by-ball_radius)<=(blocky+block_hight)))&&(((bx+ball_radius)==blockx)||((bx-ball_radius)==blockx+block_lenght))

//	  L3     L2    L1    R1    R2    R3
//	|=====|=====|=====|=====|=====|=====|
//
#define ZONE_L3 ball_neu.posx<(balk_end_left+zone_size)
#define ZONE_L2 (ball_neu.posx>(balk_end_left+zone_size))&&(ball_neu.posx<(balk_end_left+(2*zone_size)))
#define ZONE_L1 (ball_neu.posx>(balk_end_left+(2*zone_size)))&&(ball_neu.posx<(balk_end_left+(3*zone_size)))
#define ZONE_R1 (ball_neu.posx>(balk_end_left+(3*zone_size)))&&(ball_neu.posx<(balk_end_left+(4*zone_size)))
#define ZONE_R2 (ball_neu.posx>(balk_end_left+(4*zone_size)))&&(ball_neu.posx<(balk_end_left+(5*zone_size)))
#define ZONE_R3 ball_neu.posx>balk_end_left+(5*zone_size)
/* Function prototypes */
static void setup(void);
char buffer[20]={};
static void setup(void)
{
	/* Set up glcd, also sets up SPI and relevent GPIO pins */
	glcd_init();
}

uint8_t ms, ms10,ms100,sec,min,entprell, state;

volatile uint8_t ball_vert_richt, ball_horiz_richt, balk_horiz_richt, ball_refresh, balk_refresh;
volatile uint8_t ball_speed_y, ball_speed_x, ball_speed_y_counter,ball_speed_x_counter, start;
uint8_t balk_end_left, balk_end_right, balk_lenght, ball_radius,zone_size;

uint8_t test=0;

struct balls{
	
	uint8_t posx;
	uint8_t posy;
	
}ball_neu, ball_alt,balk_neu,balk_alt;

ISR (TIMER1_COMPA_vect)
{
	ms10++;
	if(entprell != 0)entprell--;
	
	//y-movement ball
	ball_speed_y_counter++;
	if((ball_speed_y_counter>=ball_speed_y&& (ball_speed_y!=0)))
	{
		ball_refresh=EIN;
		ball_speed_y_counter=0;
		if(ball_vert_richt==DOWN)
		{
			ball_neu.posy++;
		}else ball_neu.posy--;
	}
	//x-movement ball
	ball_speed_x_counter++;
	if((ball_speed_x_counter>=ball_speed_x) && (ball_speed_x!=0))
	{
		ball_speed_x_counter=0;
		//ball_refresh=EIN;
		if(ball_horiz_richt==RIGHT)
		{
			ball_neu.posx++;
		}else ball_neu.posx--;
	}
		
	if(ms10==10)	//10ms
	{
		ms10=0;
		ms100++;
	
		
	}
    if(ms100==10)	//100ms
	{
		ms100=0;
		sec++;
	}
	if(sec==60)	//Minute
	{
		sec=0;
		min++;
	}
}
const unsigned char batman[] PROGMEM= 
{ 
	//Batman
	/*0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 
	0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 
	0xff, 0xe7, 0x83, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x80, 0xc0, 0xfe, 0xfc, 0xf8, 0xf8, 0xfc, 0xfe, 0xc0, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x83, 
	0xe7, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfc, 0xfc, 
	0xf8, 0xf8, 0xf8, 0xf0, 0xf0, 0xe0, 0xe0, 0xc0, 0xc0, 0x80, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xe0, 
	0x70, 0x78, 0x3c, 0x3e, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 
	0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 
	0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 
	0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3e, 0x3c, 0x78, 0x70, 
	0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x0f, 
	0x3f, 0x7f, 0x3f, 0x3f, 0x1f, 0x1f, 0x0f, 0x0f, 0x0f, 0x0f, 
	0x0f, 0x1f, 0x1f, 0x1f, 0x3f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0x7f, 0x3f, 0x3f, 0x1f, 0x1f, 0x1f, 0x0f, 0x0f, 0x0f, 
	0x0f, 0x1f, 0x1f, 0x1f, 0x3f, 0x3f, 0x7f, 0x3f, 0x0f, 0x03, 
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x03, 0x07, 0x0f, 0x3f, 0xff, 0xff, 0x3f, 
	0x0f, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00*/
	//Woodstock
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x0c, 
	0x00, 0x00, 0x00, 0x80, 0xc0, 0xc0, 0x20, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x80, 0x80, 0x03, 0x07, 0x0c, 0x98, 0x70, 0x00, 
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x8f, 0x9d, 
	0x18, 0x10, 0x30, 0x20, 0x20, 0x20, 0x20, 0x60, 0x60, 0x40, 
	0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x18, 0x1e, 0x11, 0x01, 0x01, 0x01, 0x81, 
	0xc1, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 
	0xc0, 0xc0, 0xc0, 0xe0, 0x70, 0x3f, 0x1f, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x06, 0x07, 0x01, 0x01, 0x00, 0x08, 0x08, 0x0c, 0x0c, 
	0x0c, 0x0c, 0x0c, 0x0c, 0x1c, 0xf8, 0xf0, 0x00, 0x00, 0x1e, 
	0xff, 0xdf, 0x07, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xe0, 0x30, 0x18, 0x00, 
	0x08, 0x08, 0x10, 0x20, 0x20, 0xa0, 0xe0, 0xf0, 0x7e, 0xe3, 
	0xe0, 0xf8, 0xc0, 0x44, 0x18, 0xa1, 0xff, 0x1e, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 
	0x0e, 0x0c, 0x0c, 0x0c, 0x06, 0x06, 0x03, 0x13, 0x3b, 0x39, 
	0x29, 0x68, 0x68, 0x6f, 0x6d, 0x60, 0x67, 0x67, 0x67, 0x67, 
	0x67, 0x65, 0x35, 0x39, 0x39, 0x13, 0x06, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00
	
	};
void block1(uint8_t bx, uint8_t by)
{
	static uint8_t aktiv=1;
	const uint8_t blockx=0;
	const uint8_t blocky=1;
	const uint8_t block_lenght=12;
	const uint8_t block_hight=5;
	
	
	if(aktiv==1)//noch nie getroffen
	{
		glcd_draw_rect(blockx, blocky, block_lenght, block_hight, BLACK);
		if(ball_vert_richt==UP)//block hit from the bottom?
		{
			if(BLOCK_BOTTOM)
			{
				ball_vert_richt=DOWN;
				aktiv=0;//block verbraucht weil getroffen
				glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
			}
		}else 	if(BLOCK_TOP)
				{
					ball_vert_richt=UP;
					aktiv=0;//block verbraucht weil getroffen
					glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
				}	
		if(BLOCK_SIDE)
		{
			if(ball_horiz_richt==LEFT)
			{
				ball_horiz_richt=RIGHT;
			}else 	ball_horiz_richt=LEFT;
			aktiv=0;//block verbraucht weil getroffen, egal von welcher seite
			glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
		}
	}
}//eof Block
void block2(uint8_t bx, uint8_t by)
{
	static uint8_t aktiv=1;
	const uint8_t blockx=12;
	const uint8_t blocky=1;
	const uint8_t block_lenght=12;
	const uint8_t block_hight=5;
	
	
	if(aktiv==1)//noch nie getroffen
	{
		glcd_draw_rect(blockx, blocky, block_lenght, block_hight, BLACK);
		if(ball_vert_richt==UP)//block hit from the bottom?
		{
			if(BLOCK_BOTTOM)
			{
				ball_vert_richt=DOWN;
				aktiv=0;//block verbraucht weil getroffen
				glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
			}
		}else 	if(BLOCK_TOP)
				{
					ball_vert_richt=UP;
					aktiv=0;//block verbraucht weil getroffen
					glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
				}	
		if(BLOCK_SIDE)
		{
			if(ball_horiz_richt==LEFT)
			{
				ball_horiz_richt=RIGHT;
			}else 	ball_horiz_richt=LEFT;
			aktiv=0;//block verbraucht weil getroffen, egal von welcher seite
			glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
		}
	}
}//eof Block
void block3(uint8_t bx, uint8_t by)
{
	static uint8_t aktiv=1;
	const uint8_t blockx=24;
	const uint8_t blocky=1;
	const uint8_t block_lenght=12;
	const uint8_t block_hight=5;
	
	
	if(aktiv==1)//noch nie getroffen
	{
		glcd_draw_rect(blockx, blocky, block_lenght, block_hight, BLACK);
		if(ball_vert_richt==UP)//block hit from the bottom?
		{
			if(BLOCK_BOTTOM)
			{
				ball_vert_richt=DOWN;
				aktiv=0;//block verbraucht weil getroffen
				glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
			}
		}else 	if(BLOCK_TOP)
				{
					ball_vert_richt=UP;
					aktiv=0;//block verbraucht weil getroffen
					glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
				}	
		if(BLOCK_SIDE)
		{
			if(ball_horiz_richt==LEFT)
			{
				ball_horiz_richt=RIGHT;
			}else 	ball_horiz_richt=LEFT;
			aktiv=0;//block verbraucht weil getroffen, egal von welcher seite
			glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
		}
	}
}//eof Block
void block4(uint8_t bx, uint8_t by)
{
	static uint8_t aktiv=1;
	const uint8_t blockx=36;
	const uint8_t blocky=1;
	const uint8_t block_lenght=12;
	const uint8_t block_hight=5;
	
	
	if(aktiv==1)//noch nie getroffen
	{
		glcd_draw_rect(blockx, blocky, block_lenght, block_hight, BLACK);
		if(ball_vert_richt==UP)//block hit from the bottom?
		{
			if(BLOCK_BOTTOM)
			{
				ball_vert_richt=DOWN;
				aktiv=0;//block verbraucht weil getroffen
				glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
			}
		}else 	if(BLOCK_TOP)
				{
					ball_vert_richt=UP;
					aktiv=0;//block verbraucht weil getroffen
					glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
				}	
		if(BLOCK_SIDE)
		{
			if(ball_horiz_richt==LEFT)
			{
				ball_horiz_richt=RIGHT;
			}else 	ball_horiz_richt=LEFT;
			aktiv=0;//block verbraucht weil getroffen, egal von welcher seite
			glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
		}
	}
}//eof Block
void block5(uint8_t bx, uint8_t by)
{
	static uint8_t aktiv=1;
	const uint8_t blockx=48;
	const uint8_t blocky=1;
	const uint8_t block_lenght=12;
	const uint8_t block_hight=5;
	
	
	if(aktiv==1)//noch nie getroffen
	{
		glcd_draw_rect(blockx, blocky, block_lenght, block_hight, BLACK);
		if(ball_vert_richt==UP)//block hit from the bottom?
		{
			if(BLOCK_BOTTOM)
			{
				ball_vert_richt=DOWN;
				aktiv=0;//block verbraucht weil getroffen
				glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
			}
		}else 	if(BLOCK_TOP)
				{
					ball_vert_richt=UP;
					aktiv=0;//block verbraucht weil getroffen
					glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
				}	
		if(BLOCK_SIDE)
		{
			if(ball_horiz_richt==LEFT)
			{
				ball_horiz_richt=RIGHT;
			}else 	ball_horiz_richt=LEFT;
			aktiv=0;//block verbraucht weil getroffen, egal von welcher seite
			glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
		}
	}
}//eof Block
void block6(uint8_t bx, uint8_t by)
{
	static uint8_t aktiv=1;
	const uint8_t blockx=60;
	const uint8_t blocky=1;
	const uint8_t block_lenght=12;
	const uint8_t block_hight=5;
	
	
	if(aktiv==1)//noch nie getroffen
	{
		glcd_draw_rect(blockx, blocky, block_lenght, block_hight, BLACK);
		if(ball_vert_richt==UP)//block hit from the bottom?
		{
			if(BLOCK_BOTTOM)
			{
				ball_vert_richt=DOWN;
				aktiv=0;//block verbraucht weil getroffen
				glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
			}
		}else 	if(BLOCK_TOP)
				{
					ball_vert_richt=UP;
					aktiv=0;//block verbraucht weil getroffen
					glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
				}	
		if(BLOCK_SIDE)
		{
			if(ball_horiz_richt==LEFT)
			{
				ball_horiz_richt=RIGHT;
			}else 	ball_horiz_richt=LEFT;
			aktiv=0;//block verbraucht weil getroffen, egal von welcher seite
			glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
		}
	}
}//eof Block
void block7(uint8_t bx, uint8_t by)
{
	static uint8_t aktiv=1;
	const uint8_t blockx=72;
	const uint8_t blocky=1;
	const uint8_t block_lenght=12;
	const uint8_t block_hight=5;
	
	
	if(aktiv==1)//noch nie getroffen
	{
		glcd_draw_rect(blockx, blocky, block_lenght, block_hight, BLACK);
		if(ball_vert_richt==UP)//block hit from the bottom?
		{
			if(BLOCK_BOTTOM)
			{
				ball_vert_richt=DOWN;
				aktiv=0;//block verbraucht weil getroffen
				glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
			}
		}else 	if(BLOCK_TOP)
				{
					ball_vert_richt=UP;
					aktiv=0;//block verbraucht weil getroffen
					glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
				}	
		if(BLOCK_SIDE)
		{
			if(ball_horiz_richt==LEFT)
			{
				ball_horiz_richt=RIGHT;
			}else 	ball_horiz_richt=LEFT;
			aktiv=0;//block verbraucht weil getroffen, egal von welcher seite
			glcd_draw_rect(blockx, blocky, block_lenght, block_hight, WHITE);
		}
	}
}//eof Block


int main(void)
{	
	
	
	/* Backlight pin PL3, set as output, set high for 100% output */
	DDRB |= (1<<PB2);
	PORTB |= (1<<PB2);
	
	DDRC |=(1<<PC3); 	//Ausgang LED
	PORTC |= (1<<PC3);	//Led ein
		
	DDRD &= ~((1<<PD6) | (1<<PD2) | (1<<PD5)); 	//Taster 1-3
	PORTD |= ((1<<PD6) | (1<<PD2) | (1<<PD5)); 	//PUllups für Taster einschalten
	
	//Timer 1 Configuration
	OCR1A = 0x009C;	//OCR1A = 0x3D08;==1sec
	
    TCCR1B |= (1 << WGM12);
    // Mode 4, CTC on OCR1A

    TIMSK1 |= (1 << OCIE1A);
    //Set interrupt on compare match

    TCCR1B |= (1 << CS12) | (1 << CS10);
    // set prescaler to 1024 and start the timer

    sei();
    // enable interrupts
 
	
	setup();
	
	glcd_clear();
	glcd_write();
	
	balk_lenght=16;//lenght of balken
	
	balk_neu.posx = 8;//linke seite balken startposition
	ball_vert_richt = UP; //0=down 1=up
	ball_horiz_richt = LEFT; 
	balk_horiz_richt=LEFT;//0=left to right, 1=right to left
	ball_speed_y=0;
	ball_speed_x=0;
	ball_speed_y_counter=0;
	
	balk_neu.posy=46; //Startposition bottom of screen
	zone_size=5;
	balk_lenght=6*zone_size;//lenght of balken
	balk_end_left=balk_neu.posx;
	balk_end_right=balk_neu.posx+balk_lenght;
	ball_neu.posx=balk_neu.posx+(balk_lenght/2);
	ball_neu.posy = balk_neu.posy-ball_radius;//mittelpunkt ball startposition
	ball_radius=4;
	ball_refresh=EIN;
	balk_refresh=EIN;
	start=1;//Start of new game
	// Display
	glcd_tiny_set_font(Font5x7,5,7,32,127);
	glcd_clear_buffer();

	state=8;
	while(1) 
	{
		
		block1(ball_neu.posx, ball_neu.posy);
		block2(ball_neu.posx, ball_neu.posy);
		block3(ball_neu.posx, ball_neu.posy);
		block4(ball_neu.posx, ball_neu.posy);
		block5(ball_neu.posx, ball_neu.posy);
		block6(ball_neu.posx, ball_neu.posy);
		block7(ball_neu.posx, ball_neu.posy);
		
		if(T_RED)//move right
		{
			entprell=RELOAD_ENTPRELL;
			if((balk_neu.posx+balk_lenght)!=84)balk_neu.posx++;
			balk_horiz_richt=LEFT;
			balk_refresh=EIN;
		}
		if(T_BLUE)//move left
		{
			entprell=RELOAD_ENTPRELL;
			if(balk_neu.posx!=0)balk_neu.posx--;
			balk_horiz_richt=RIGHT;
			balk_refresh=EIN;
		}
		if(T_GREEN)//move left
		{
			entprell=RELOAD_ENTPRELL;
			ball_speed_y=6;
			start=0;
		}
		//Balken Endpunkte neu berechnen
		balk_end_left=balk_neu.posx;
		balk_end_right=balk_neu.posx+balk_lenght;
		
		
		if(ball_neu.posy==4)ball_vert_richt=DOWN;//reached top screen border
		
		//bounce ball on balk
		if((ball_neu.posy==41)&&((ball_neu.posx+ball_radius)>balk_end_left)&&((ball_neu.posx-ball_radius)<balk_end_right))
		{
			ball_vert_richt=UP;
			balk_refresh=EIN;
			if(ZONE_L3)
			{
				ball_speed_x=2;
				ball_horiz_richt=LEFT;
			}else	if(ZONE_L2)
					{
						ball_speed_x=4;
							ball_horiz_richt=LEFT;
					}else	if(ZONE_L1)
						{
							ball_speed_x=8;
								ball_horiz_richt=LEFT;
						}else	if(ZONE_R1)
								{
									ball_speed_x=8;
									ball_horiz_richt=RIGHT;
								}else	if(ZONE_R2)
										{
											ball_speed_x=4;
											ball_horiz_richt=RIGHT;
										}else	if(ZONE_R3)
												{
													ball_speed_x=2;
													ball_horiz_richt=RIGHT;
												}
					
			
		}else
		
		if(ball_neu.posx-ball_radius==0)ball_horiz_richt=RIGHT;
		if(ball_neu.posx+ball_radius==84)ball_horiz_richt=LEFT;
		
		if(start==1)
		{
			
			ball_alt=ball_neu;
			ball_refresh=EIN;
			balk_refresh=EIN;
			ball_neu.posx=balk_neu.posx+(balk_lenght/2);
			ball_neu.posy = balk_neu.posy-ball_radius;//mittelpunkt ball startposition
			//sprintf(buffer, "pos=%.d",test);
			//glcd_draw_string_xy(20, 10, buffer);
		}
		
		/*
		sprintf(buffer, "b_pos_x=%d", ball_neu.posy);
		glcd_draw_string_xy(10, 0, buffer);
		
		sprintf(buffer, "end_left=%.d", balk_end_left);
		glcd_draw_string_xy(20, 10, buffer);
		
		sprintf(buffer, "end_right%.d", balk_end_right);
		glcd_draw_string_xy(20, 20, buffer);
		*/
		
		
		if((ball_refresh==EIN)||(balk_refresh==EIN))//only draw if anything has changed
		{
			if(ball_refresh==EIN)//only draw ball when position changed
			{
				ball_refresh=AUS;
				
				glcd_draw_circle(ball_alt.posx, ball_alt.posy, ball_radius, WHITE);
				glcd_draw_circle(ball_neu.posx, ball_neu.posy, ball_radius, BLACK);
				ball_alt=ball_neu;//save new ball position
			}
			
			if(balk_refresh==EIN)//only draw balk when position changed
			{
				balk_refresh=AUS;
				//delete old plank draw new one
				if(balk_horiz_richt==LEFT)
				{
					glcd_draw_rect(balk_alt.posx, balk_neu.posy, balk_lenght, 2, WHITE);
					glcd_draw_rect(balk_neu.posx, balk_neu.posy, balk_lenght, 2, BLACK);
				}else glcd_draw_rect(balk_alt.posx, balk_neu.posy, balk_lenght, 2, WHITE);glcd_draw_rect(balk_neu.posx, balk_neu.posy, balk_lenght, 2, BLACK);
				balk_alt=balk_neu;
			}
			glcd_write();
		}
		
	
	}//End of while
	
	//---------------------------------------------
	
	return 0;
}//end of main
