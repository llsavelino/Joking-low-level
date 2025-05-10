#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "registerB.h"

// Protótipo correto da função
void toggle(void); void analog(volatile uint8_t, volatile int);
volatile uint8_t pwm = 0; volatile int posineg = 1;

// Inicializa a tarefa
operatingSystem tasks[NUM_TASKS] = {
    { .funcSp = toggle, .SPorCP = false, .interval_ms = 0x1F4, .counter = 0x00, .ok = false, .padding = 
        {
            0b00000000,                                                                    0b00000000
        } 
    }
    ,
    { .funcCp = analog, .SPorCP = true, .interval_ms = 0x1F4, .counter = 0x00, .ok = false, .padding =
        {
            0b00000000,                                                                    0b00000000
        }
    }
};

void setup(void) {
    // Configura PB5 como saída
    DDRB |=   ((0x01 << PB5) | (0x01 << PB0)); PORTB_REG.pb5 = 0b0; PORTB_REG.pb0 = 0b0;

    // Configura Timer1 em modo CTC com prescaler 64
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (0 << CS12) |(1 << CS11) | (1 << CS10); // 64x prescaler, modo CTC
    OCR1A = 250; // 16MHz / 64 = 250kHz → 250 ciclos = 1 ms
    TIMSK1 |= (1 << OCIE1A); // Habilita interrupção por comparação A

    // --- Configura Timer2 para Fast PWM no pino OC2B ---
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20); // Fast PWM, não-invertido
    TCCR2B = (1 << CS21); /* Prescaler = 8 */ OCR2B = pwm;

}

void loop(void) {
    for (int i = 0; i < NUM_TASKS; ++i) {
        if (tasks[i].ok) {
            if (!tasks[i].SPorCP) tasks[i].funcSp();
            else      tasks[i].funcCp(pwm, posineg);
            tasks[i].ok = false;
        }
    }
}

// Interrupção de Timer1 a cada 1ms
ISR(TIMER1_COMPA_vect) {
    for (int i = 0; i < NUM_TASKS; ++i) {
        tasks[i].counter++;
        if (tasks[i].counter >= tasks[i].interval_ms) {
            tasks[i].counter = 0x00; tasks[i].ok = 0x01;
        }
    }
}

// Alterna o estado de PB5 (LED)
void toggle(void) { PORTB_REG.pb5 ^= 0x01; }
void analog(volatile uint8_t i, volatile int y) { 
  i += y; if (i == 0xff || i == 0x00) { y = -y; } OCR2B = i;
}
