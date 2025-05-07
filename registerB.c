#include <avr/io.h>
#include <avr/interrupt.h>
#include "registerB.h"

// Implementação do setup
void setup(void) {
     DDRB &=  ~( 1 << PB5 );
     DDRB |=   ( 1 << PB5 );
     PORTB &= ~( 1 << PB5 );

    TCCR1A = 0x00; // Modo normal
    TCCR1B = (1 << WGM12) | (1 << CS12) | (0 << CS11) | (1 << CS10); // CTC, prescaler 64
    OCR1A = 15624 >> 1; // Conta até 249 (1ms) para calcular o segundo multiplicar com 1000
    TIMSK1 |= (1 << OCIE1A); // Habilita interrupção de comparação A
}

// Implementação do loop
void loop(void) { }

// Manipulador de interrupção
ISR(TIMER1_COMPA_vect) {
    static uint8_t timeCount = 0x00;
    ++timeCount;
    if (timeCount == 0xCB) {
        PORTB_REG.pb5 ^= 0x01;
    }
}
