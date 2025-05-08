#include <avr/io.h>
#include <avr/interrupt.h>
#include "registerB.h"

// Protótipo correto da função
void swpa(void);

// Inicializa a tarefa
Task tasks[NUM_TASKS] = {{ swpa, 500, 0x00, 0x00 }};

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
    if (tasks[0x00].ok) {
        tasks[0x00].ok = 0x00;
        tasks[0x00].func(); // Executa a tarefa
    }
}

// Interrupção de Timer1 a cada 1ms
ISR(TIMER1_COMPA_vect) {
    tasks[0x00].counter++;
    if (tasks[0x00].counter >= tasks[0x00].interval_ms) {
        tasks[0x00].counter = 0x00;
        tasks[0x00].ok = 0x01;
    }
}

// Alterna o estado de PB5 (LED)
void swpa(void) { PORTB_REG.pb5 ^= 0x01; }
