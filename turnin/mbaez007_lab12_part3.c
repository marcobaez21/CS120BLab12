/*	Author: Marco Baez
 *  Partner(s) Name: NA
 *	Lab Section:021
 *	Assignment: Lab #12  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo link: https://drive.google.com/drive/folders/1-1_Zf0EL01XEZT5VoUOW-SXjQH8O5fE9?usp=sharing
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


typedef struct task{
        int state;
        unsigned long period;
        unsigned long elapsedtime;
        int(*TickFct)(int);
} task;

task tasks[1];

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){

        TCCR1B = 0x0B;

        OCR1A = 125;

        TIMSK1 = 0x02;

        TCNT1 = 0x02;

        _avr_timer_cntcurr = _avr_timer_M;

        SREG |= 0x80;
}

void TimerOff(){

        TCCR1B = 0x00;

}

void TimerISR(){
        //TimerFlag = 1;
        unsigned char i;
        for(i=0;i<1;i++){
                if(tasks[i].elapsedtime>=tasks[i].period){
                        tasks[i].state=tasks[i].TickFct(tasks[i].state);
                        tasks[i].elapsedtime=0;
                }
                tasks[i].elapsedtime+=1;
        }
}

ISR(TIMER1_COMPA_vect){
        _avr_timer_cntcurr--;
        if(_avr_timer_cntcurr == 0){
                TimerISR();
                _avr_timer_cntcurr = _avr_timer_M;
        }
}

void TimerSet(unsigned long M){
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}

unsigned char button = 0x00;
unsigned char button2 = 0x00;
enum states{StartLED};
int tick(int state){
	button=~PINA&0x01;
	button2=(~PINA>>1)&0x01;

	static unsigned char pattern[3]  = {0x3C, 0x24, 0x3C}; //fix these two values later
	static unsigned char row[3] = {0xF7, 0xFB, 0xFD}; 
	
	static unsigned char temp_pattern = 0x00;
	static unsigned char temp_row = 0x00;
	static unsigned char count = 0x00;

	switch(state){
		case StartLED:
			temp_pattern=pattern[count]; 
			temp_row=row[count]; 
			count++;
			if(count==3){count=0;}
	}
	PORTC=temp_pattern;
	PORTD=temp_row;
	return state;
}

int main(void) {
    DDRA=0x00; PORTA=0xFF;
    DDRC=0xFF; PORTC=0x00;
    DDRD=0xFF; PORTD=0x00;

    unsigned char temp = 0x00;
    tasks[temp].state=StartLED;
    tasks[temp].period=1;
    tasks[temp].elapsedtime=0;
    tasks[temp].TickFct=&tick;  
    
    TimerSet(1);
    TimerOn(); 
   
    while (1) {
//	PORTC=0xFF;
//	PORTD=0xFB;;
    }
    return 1;
}
