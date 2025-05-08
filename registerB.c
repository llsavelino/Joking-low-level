#include <avr/io.h>
#include <avr/interrupt.h>
#include "registerB.h"

static volatile uint8_t pwm = 0x00;
static volatile int direcao = 0x01;

// Implementação do setup
void setup(void) {
     DDRB |= (( !0x00 << PB5 ) | ( !0x00 << PB4 ));
     PORTB &= ~(( 0x01 << PB5 ) | ( 0x01 << PB4 ));

    TCCR1A = 0x00; // Modo normal
    TCCR1B = (0x01 << WGM12) | (0x01 << CS12) | (0x00 << CS11) | (0x01 << CS10); // CTC, prescaler 64
    OCR1A = 0xff; // Conta até 255 (?) para calcular o ? multiplicar com ?
    TIMSK1 |= (0x01 << OCIE1A); // Habilita interrupção de comparação A

    // --- Configura Timer2 para Fast PWM no pino OC2B ---
    TCCR2A = (0x01 << COM2B1) | (0x01 << WGM21) | (0x01 << WGM20); // Fast PWM, não-invertido
    TCCR2B = (0x01 << CS21); // Prescaler = 8
    OCR2B = pwm;
}

// Implementação do loop
void loop(void) { }

// Manipulador de interrupção
ISR(TIMER1_COMPA_vect) {
    static uint8_t timeCount = 0x00;
    ++timeCount;
    if (timeCount == 0xCB) { PORTB_REG.pb5 ^= 0x01; timeCount = 0x00; }
    pwm += direcao;
    if (pwm == 0xff || pwm == 0x00) { direcao = -direcao; /* Inverte direção quando atinge os limites */ }
    OCR2B = pwm;
}
