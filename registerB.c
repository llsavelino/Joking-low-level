#include <avr/io.h>
#include <avr/interrupt.h>
#include "registerB.h"

// Protótipo correto da função
void toggle(void);

// Inicializa a tarefa
operatingSystem tasks[NUM_TASKS] = {
    { .func = toggle, .SPorCP = false, .interval_ms = 0x1F4, .counter = 0x00, .ok = false, .padding = 
        {
            0b00000000, 0b00000000, 0b00000000
        } 
    }
};

void setup(void) {
    // Configura PB5 como saída
    DDRB |=   (0x01 << PB5);
    PORTB &= ~(0x01 << PB5); // Começa desligado

    // Configura Timer1 em modo CTC com prescaler 64
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (0 << CS12) |(1 << CS11) | (1 << CS10); // 64x prescaler, modo CTC
    OCR1A = 250; // 16MHz / 64 = 250kHz → 250 ciclos = 1 ms
    TIMSK1 |= (1 << OCIE1A); // Habilita interrupção por comparação A

    sei(); // Habilita interrupções globais
}

void loop(void) {
    for (int i = 0; i < NUM_TASKS; ++i) {
        if (tasks[i].ok && !tasks[i].SPorCP) { tasks[i].ok = 0x00; tasks[i].func(); }
    }
}

// Interrupção de Timer1 a cada 1ms
ISR(TIMER1_COMPA_vect) {
    for (int i = 0; i < NUM_TASKS; ++i) {
        tasks[i].counter++;
        if (tasks[i].counter >= tasks[i].interval_ms) {
            tasks[i].counter = 0x00;
            tasks[i].ok = 0x01;
        }
    }
}

// Alterna o estado de PB5 (LED)
void toggle(void) { PORTB_REG.pb5 ^= 0x01; }
