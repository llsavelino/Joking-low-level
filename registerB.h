#pragma once
#ifndef REGB_H
#define REGB_H
#include        <avr/io.h>
#include        <stdint.h>
#include       <stdbool.h>
#include <avr/interrupt.h>

typedef union {
    signed int: 0x00;
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

#define NUM_TASKS  0x03
#define QUEUE_SIZE 0x03
#define COLUMN     0x02
#define LINE       0x03
#ifndef REGISTERS_AVR
#define REGISTERS_AVR
#define PORTB_REG (* (volatile BITSregPortB_t *)&PORTB)
#define PORTC_REG (* (volatile BITSregPortB_t *)&PORTC)
#define PORTD_REG (* (volatile BITSregPortB_t *)&PORTD)
#define DDRB_REG  (* (volatile BITSregPortB_t *) &DDRB)
#define DDRC_REG  (* (volatile BITSregPortB_t *) &DDRC)
#define DDRD_REG  (* (volatile BITSregPortB_t *) &DDRD)
#define SPINB_REG (* (volatile BITSregPortB_t *) &PINB)
#define SPINC_REG (* (volatile BITSregPortB_t *) &PINC)
#define SPIND_REG (* (volatile BITSregPortB_t *) &PIND)
#endif
#undef  REGISTERS_AVR

typedef struct {
    long: 0x00;
    union {
      void (*funcSp)                          (void);          // Ponteiro para a função da tarefa
      void (*funcCp)(volatile uint8_t, volatile int);
    };
    unsigned int      interval_ms;       // Intervalo de execução em ms
    volatile unsigned int counter;  // Contador da tarefa
    volatile bool          ok;                // Flag de execução
    uint8_t padding[  0x03  ]; // aproveitamento de pedaço para flags de usos especiais
} operatingSystem; extern operatingSystem tasks[NUM_TASKS];

typedef struct {
    int: 0x00;
    uint8_t buffer[QUEUE_SIZE];
    uint8_t head;  // Índice para escrever
    uint8_t tail;  // Índice para ler
    uint8_t count; // Número de elementos na fila
} CircularQueue; extern CircularQueue queueOS; 

void InitQueue(CircularQueue *queue) 
{
    queue->head  = 0;
    queue->tail  = 0;
    queue->count = 0;

    for (int i = 0; i < QUEUE_SIZE; ++i) 
    {
        queue->buffer[i] = 0;
    }
}

bool QueueisEmpty(const CircularQueue* Q) { return Q->count == 0b00000000; }
bool Queueis_Full(const CircularQueue* Q) { return Q->count == QUEUE_SIZE; }

bool queueEnqueue(CircularQueue* queue, const uint8_t data)
{
    if (Queueis_Full(queue)) return false;
    queue->buffer[queue->head] = data;
    queue->head = (queue->head +1) % QUEUE_SIZE;
    queue->count ++; return true;
}

bool queue_enqueue_overwrite(CircularQueue* queue, const uint8_t data)
{
    if (Queueis_Full(queue))
    {
        // Avança o tail para "descartar" o dado mais antigo
        queue->tail = (queue->tail +1) % QUEUE_SIZE;
        queue->count --;
    }

    queue->buffer[queue->head] = data;
    queue->head = (queue->head +1) % QUEUE_SIZE;
    queue->count ++; return true;
}

bool queueDequeue(CircularQueue* queue, uint8_t* ptrData)
{
    if (QueueisEmpty(queue)) return false;
    *ptrData = queue->buffer[queue->tail];
    queue->tail = (queue->tail +1) % QUEUE_SIZE;
    queue->count --; return true;
}

bool queuePeekfirst(const CircularQueue* queue, uint8_t* ptrData)
{
    if (QueueisEmpty(queue)) return false;
    *ptrData = queue->buffer[queue->tail]; return true;
}

bool queuePeeklast(const CircularQueue* queue, uint8_t* ptrData)
{
    if (QueueisEmpty(queue)) return false;
    uint8_t lastIndex = (queue->head == 0) ? (QUEUE_SIZE -1): (queue->head -1);
    *ptrData = queue->buffer[lastIndex]; return true;
}

bool queuePeekmiddle(const CircularQueue* queue, uint8_t* ptrData)
{
    if (QueueisEmpty(queue)) return false;
    uint8_t offset = queue->count / 0x02, index = (queue->tail +offset) % QUEUE_SIZE;
    *ptrData = queue->buffer[index]; return true;
}

#ifdef __cplusplus
extern "C" {
#endif
void setup(void); void loop(void);
#ifdef __cplusplus
}
#endif
#endif // REGB_H
