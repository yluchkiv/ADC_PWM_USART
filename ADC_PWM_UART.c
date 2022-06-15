// just a very basic comment

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#define BUAD 9600
#define BRC ((F_CPU/16/BUAD)-1)  // = 103.16 //

double duty_cycle_PD6 = 0; // value for PWM  PD6
double duty_cycle_PD3 = 0; // value for PWM  PD3

void start_ADC();//prototype 


void timer0_set()
{
    DDRD |= (1 << PORTD6); // port D6 output = OC0A

    TCCR0A = (1 << COM0A1)|(1 << WGM00)|(1 << WGM01); // selection PWM fast for OC0A and wave gen. mode

    TCCR0B = (1 << CS00)|(1 << CS02); // prescaler 1024, so real time 0.01 sec and clk start
}

void timer2_set()
{
    DDRD |= (1 << PORTD3); // port D3 output = OC2B

    TCCR2A = (1 << COM2B1)|(1 << WGM20)|(1 << WGM21); // selection PWM fast for OC2B and wave gen. mode
    
    TCCR2B = (1 << CS20)|(1 << CS21)|(1 << CS22);// prescaler 2024 and clk start
}

void set_ADC()
{
    ADMUX = (1 << REFS0)|(1 << ADLAR)|(1 << MUX0)|(1 << MUX2); // ref voltage | ned only 8bit ADC from ADCH| pin ADC5 selection
    ADCSRA = (1 << ADEN)|(1 << ADIE)|(1 << ADPS0)|(1 << ADPS1)|(1 << ADPS2);// ADC enable; !!!ADC interrupt when conversion is done!!!;prescaler 128
    DIDR0 = (1 << ADC5D); // turn off the digital input for ADC5 pin

    start_ADC();

}

void start_ADC()
{
    ADCSRA |= (1 << ADSC); //  starts conversion
}

void usart_set()
{
    UBRR0H = (BRC >> 8);  // ask Volodya
    UBRR0L = BRC;

    UCSR0B = (1 << TXEN0); // transmit enable
    UCSR0C = (1 << UCSZ01)|(1 << UCSZ00); //set 8bit
}

void print_double(double value)
{
    char str[32]={'\0',}; //string with initialized "0" inside
    snprintf ( str,sizeof(str), "%5.1f\r\n", value );
    int len = strlen(str);

    for( int i = 0; i < len; i++)
    {
        /* Wait for empty transmit buffer */
        while ( !( UCSR0A & (1<<UDRE0)) )
        ;
        /* Put data into buffer, sends the data */
        UDR0 = str[i];
        
    }


}

int main()
{
    timer0_set();
    timer2_set();

    set_ADC();
    usart_set();

    sei();

    while(1)
    {
        //UDR0 = (uint8_t)(duty_cycle_PD6/255*10)+48; // max 255
        double value_PD6 = 0;

        cli();
        value_PD6 = duty_cycle_PD6;
        sei();

        print_double(value_PD6);
       // print_double(duty_cycle_PD3);
        _delay_ms(1000);
        

    }
}
ISR(ADC_vect)
{
    duty_cycle_PD6 = ADCH; //   01100110 goes to double, double has its own conversion format
    OCR0A = duty_cycle_PD6; // duty cycle for PD6

    duty_cycle_PD3 = ADCH;
    OCR2B = duty_cycle_PD3; //duty cycle for PD3

   

    start_ADC(); // restarts conversion after the previous was completed.
  
}

