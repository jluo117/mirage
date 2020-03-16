#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "bit.h"
#include "usart.h"

enum States {init, start, on, off} state;
unsigned char sound = 0x00; //ADC0
unsigned char y = 0x00; //ADC1
unsigned char count = 0;


void ADC_init() {
	DDRA=0x0;			// Make ADC port as input
	ADCSRA = 0x87;			//Enable ADC, fr/128  
	ADMUX = 0x40;			// Vref: Avcc, ADC channel: 0
}

int ADC_Read(char channel) {
	int Ain,AinLow;

	ADMUX=ADMUX|(channel & 0x0f);	//Set input channel to read

	ADCSRA |= (1<<ADSC);		//Start conversion
	while((ADCSRA&(1<<ADIF))==0);	//Monitor end of conversion interrupt

	AinLow = (int)ADCL;		//Read lower byte
	Ain = (int)ADCH*256;		//Read higher 2 bits and Multiply with weight
	Ain = Ain + AinLow;
	return(Ain);			//Return digital value
}

/*
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}*/

void tick() {
	switch(state) {
		case init:
			state = start;
			while (!USART_IsSendReady());
			USART_Send(0);
			while(!USART_HasTransmitted());
			break;
		case start:
			count++;
			if(count == 20) {
				state = on;
			}
			else {
				state = start;
			}
			break;
		case on:
		count++;
			if(count == 2400) {
				state = off;
			}
			else {
				state = on;
			}
			break;
		case off:
			count++;
			if(count == 2450) {
				count = 0;
				state = start;
			}
			else {
				state = off;
			}
			break;
		default:
			state = init;
			break;
	}
	switch (state) {
		case init:
			break;
		case start:
			break;
		case on:
			sound = ADC;
			//sound = ADC_Read(0);
			//y = ADC_Read(1);
			break;
		case off:
			sound = 0x0000;
			break;
		default:
			break;
	}
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF; // input on A
	DDRB = 0xFF; PORTB = 0x00; //output on B

	ADC_init();
	TimerSet(100);
	TimerOn();
	initUSART();
	USART_Flush();
	state = init;

	while(1) {
		tick();
		while (!USART_IsSendReady());
		//USART_Send((sound & 0x00FF)); //microphone

		//USART_Send((sound & 0x00FF)); //mic

		//if (ADC_Read(1) < 0x08 && ((sound & 0x00FF) > 0x03F)) { //left
			USART_Send(ADC_Read(1));
		//}
		while(!USART_HasTransmitted());
		while (!TimerFlag){}
		TimerFlag = 0;
		/*
		unsigned short my_short = ADC; //sample sound
		if (my_short >= 0x00) {
			light = 0x00;
		}
		if (my_short >= 0xF8/2) {
			light = ~light;6
		}
		PORTB = ~light;
		*/
	}
	return 1;

}
