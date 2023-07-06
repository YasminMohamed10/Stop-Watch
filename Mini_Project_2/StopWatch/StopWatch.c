/********************************************/
/* Description : MINI_PROJECT_2             */
/* Author      : Yasmin Mohamed             */
/* Date        : 16/9/2022                  */
/* Version     : 0.1V                       */
/********************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

unsigned int TIM1_Interrupt_Flag;
unsigned int Sev_Seg[6];
unsigned int counter;

ISR(TIMER1_COMPA_vect)
{
	TIM1_Interrupt_Flag=1;
}

ISR(INT0_vect)
{
	/* Reset Stop Watch */
	for(counter=0; counter<6; counter++)
	{
		Sev_Seg[counter]=0;
	}
}
ISR(INT1_vect)
{
	/* Pause Stop Watch */
	TCCR1B &= ~(1<<CS12);
	TCCR1B &= ~(1<<CS11);
	TCCR1B &= ~(1<<CS10);
}
ISR(INT2_vect)
{
	/* Resume Stop Watch */
	TCCR1B|=(1<<CS10);
	TCCR1B|=(1<<CS12);
}

void TIMER1_CTC_Init(void)
{
	TCNT1 = 0;
	OCR1A = 977 ;
	TIMSK |= (1<<OCIE1A);
	TCCR1A = (1<<FOC1A);

	/*
	 * CTC Mode
	 * Prescaler = F_CPU / 1024;
	*/
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
	SREG|=(1<<7);

}

void INT0_Init(void)
{
	DDRD  &= (~(1<<PD2))     ;
	PORTD |= (1<<PD2)        ;
	MCUCR |= (1<<ISC01)      ;
	MCUCR &= ~(1<<ISC00)     ;
	GICR  =  GICR | (1<<INT0);
	SREG  =  SREG | (1<<7)   ;
}

void INT1_Init(void)
{
	DDRD  &= (~(1<<PD3))     ;
	MCUCR |= (1<<ISC10)      ;
	MCUCR |= (1<<ISC11)      ;
	GICR  =  GICR | (1<<INT1);
	SREG  =  SREG | (1<<7)   ;
}

void INT2_Init(void)
{
	DDRD   &= (~(1<<PB2))    ;
	PORTB  |= (1<<PB2)       ;
	MCUCSR &= (~(1<<ISC2))   ;
	GICR  =  GICR | (1<<INT2);
	SREG  =  SREG | (1<<7)   ;
}

void Seven_Segment(void)
{
	Sev_Seg[0]++;
	if(Sev_Seg[0]==10)
	{
		Sev_Seg[0]=0;
		Sev_Seg[1]++;
		if(Sev_Seg[1]==6 && Sev_Seg[0]==0)
		{
			Sev_Seg[1]=0;
			Sev_Seg[2]++;
		}
	}
	if(Sev_Seg[2]==10)
	{
		Sev_Seg[2]=0;
		Sev_Seg[3]++;
		if(Sev_Seg[3]==6 && Sev_Seg[2]==0)
		{
			Sev_Seg[3]=0;
			Sev_Seg[4]++;
		}
	}
	if(Sev_Seg[4]==10)
	{
		Sev_Seg[4]=0;
		Sev_Seg[5]++;
		if(Sev_Seg[5]==10 &&Sev_Seg[4]==0)
		{

			for(counter=0;counter<6;counter++)
			{
				Sev_Seg[counter]=0;
			}
			TCNT1=0;
		}
	}

}

void main()
{
	DDRA |= 0x3F;
	DDRC |= 0x0F;
	INT0_Init();
	INT1_Init();
	INT2_Init();
	TIMER1_CTC_Init();


	while(1)
	{
		 for(counter=0;counter<6;counter++)
		 {
			PORTA=1<<counter;
			PORTC=Sev_Seg[counter];
			_delay_ms(2);
		 }

		if(TIM1_Interrupt_Flag==1)
		{
			Seven_Segment();
			TIM1_Interrupt_Flag=0;
		}
	}
}
