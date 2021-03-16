/*	Author: Marco Baez
 *  Partner(s) Name: NA
 *	Lab Section:021
 *	Assignment: Lab #12  Exercise #1
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
                tasks[i].elapsedtime+=100;
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
enum states{StartLED, UpLED, DownLED};
int tick(int state){
	button=~PINA&0x01;
	button2=(~PINA>>1)&0x01;

	static unsigned char pattern  = 0xFF; //fix these two values later
	static unsigned char row = 0xFE; 

	switch(state){
		case StartLED:
			if(button&&!button2){
				state=UpLED;
				if(row!=0xFE){row=(row>>1)|0x80;}
			}
			else if(!button&&button2){
				state=DownLED;
				if(row!=0xEF){row=(row<<1)|0x01;}
			}
			else{state=StartLED;}
			break;
		case UpLED:
			if(button&&!button2){state=UpLED;}
			else{state=StartLED;}
			break;
		case DownLED:
			if(!button&&button2){state=DownLED;}
			else{state=StartLED;}
			break;
		default:
			break;
	}
	PORTC=pattern;
	PORTD=row;
	return state;
}

int main(void) {
    DDRA=0x00; PORTA=0xFF;
    DDRC=0xFF; PORTC=0x00;
    DDRD=0xFF; PORTD=0x00;

    unsigned char temp = 0x00;
    tasks[temp].state=StartLED;
    tasks[temp].period=100;
    tasks[temp].elapsedtime=0;
    tasks[temp].TickFct=&tick;  
    
    TimerSet(100);
    TimerOn(); 
   
    while (1) {
//	PORTC=0xFF;
//	PORTD=0xFB;;
    }
    return 1;
}
