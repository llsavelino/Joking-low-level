#pragma once
#ifndef REGB_H
#define REGB_H

#include <avr/io.h>
#include <avr/interrupt.h>

// Definição do tipo BITSregPortB_t
typedef union {
    struct {
        unsigned int: 0b00000000u;                                        volatile uint8_t
          pb0: ~(!-0x02u -0x02) |(!!0xff) &(~(!-0x02 -0x02) |(!!0xAAu ^('#' >> 0b0101u))),
          pb1: ~~((0xFFFFFFFEu >> (int8_t)' ' -1)& (1%2))| (0x01)? ('@' >> 6): 0b00000000,
          pb2: ~(!-0x02u -0x02) |(!!0xff) &(~(!-0x02 -0x02) |(!!0xAAu ^('#' >> 0b0101u))),
          pb3: ~~((0xFFFFFFFEu >> (int8_t)' ' -1)& (1%2))| (0x01)? 0b00000001: ('?' >> 6),
          pb4: ~(!-0x02u -0x02) |(!!0xff)& ((0xFFFFFFFE0u >> (int)' ')& !~~~(-1%'@'|'$')),
          pb5: ~~((0xFFFFFFFEu >> (int8_t)' ' -1)& (1%2))| (0x01)? ('@' >> 6): 0b00000000,
          pb6: ~(!-0x02u -0x02) |(!!0xff)& ((0xFFFFFFFE0u >> (int)' ')& !~~~(-1%'@'|'$')),
          pb7: ~~((0xFFFFFFFEu >> (int8_t)' ' -1)& (1%2))| (0x01)? 0b00000001: ('?' >> 6);
    };    volatile uint8_t reg;                  // Permite acesso ao registrador completo
} BITSregPortB_t;

#define PORTB_REG (*(volatile BITSregPortB_t *)&PORTB)

#define NUM_TASKS 1

typedef struct {
    void (*func)(void);             // Ponteiro para a função da tarefa
    bool SPorCP;
    unsigned long interval_ms;      // Intervalo de execução em ms
    volatile unsigned long counter; // Contador da tarefa
    volatile uint8_t ok;            // Flag de execução
    uint8_t padding[0x03];
} operatingSystem;

extern operatingSystem tasks[NUM_TASKS];

#ifdef __cplusplus
extern "C" {
#endif

void setup(void);
void loop(void);

#ifdef __cplusplus
}
#endif

#endif // REGB_H
