#pragma once
#ifndef REGB_H
#define REGB_H
#include        <avr/io.h>
#include        <stdlib.h>
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

// Estrutura para cada elemento do buffer
typedef struct {
    union {
        void (*funcSp)                          (void);
        void (*funcCp)(volatile uint8_t, volatile int);
        void (*funcCpS)                  (const char*);
        void (*funcCpI)                          (int);
        int (*IntfuncCp0)                       (void);
        int (*IntfuncCp1)                        (int);
    };
    unsigned int interval_ms; // Intervalo de execução em ms
    volatile unsigned int counter; // Contador da tarefa
    volatile bool ok; // Flag de execução
    uint8_t padding[3]; // Aproveitamento para flags
} operatingSystem;

extern operatingSystem tasks[NUM_TASKS];
extern operatingSystem watchView;

// Estrutura da fila circular
typedef struct {
    operatingSystem* buffer[QUEUE_SIZE]; // Array de ponteiros para operatingSystem
    uint8_t head;  // Índice para escrever
    uint8_t tail;  // Índice para ler
    uint8_t count; // Número de elementos na fila
} CircularQueue;

extern CircularQueue queueOS;

// Inicializa a fila
void InitQueue(CircularQueue *queue) 
{
    queue->head  = 0;
    queue->tail  = 0;
    queue->count = 0;

    for (uint8_t i = 0; i < QUEUE_SIZE; ++i) 
    {
        queue->buffer[i] = NULL; // Inicializa os ponteiros como NULL
    }
    return;
}

// Libera a memória de todos os elementos na fila
void FreeQueue(CircularQueue *queue)
{
    for (uint8_t i = 0; i < QUEUE_SIZE; ++i) 
    {
        if (queue->buffer[i] != NULL)
        {
            free(queue->buffer[i]);
            queue->buffer[i] = NULL;
        }
    }
    queue->head  = 0;
    queue->tail  = 0;
    queue->count = 0;
    return;
}

// Verifica se a fila está vazia
static bool QueueisEmpty(const CircularQueue* Q) { return Q->count == 0b00000000; }
// Verifica se a fila está cheia
static bool Queueis_Full(const CircularQueue* Q) { return Q->count == QUEUE_SIZE; }

// Adiciona um elemento à fila
bool queueEnqueue(CircularQueue* queue, const operatingSystem* data)
{
    if (Queueis_Full(queue)) return false;

    if (queue->buffer[queue->head] != NULL)
    {
        free(queue->buffer[queue->head]);
        queue->buffer[queue->head] = NULL;
    }

    queue->buffer[queue->head] = data;
    queue->head = (queue->head + 1) % QUEUE_SIZE;
    queue->count++;
    return true;
}

// Adiciona um elemento à fila, sobrescrevendo se cheia
bool queue_enqueue_overwrite(CircularQueue* queue, const operatingSystem* data)
{
    if (Queueis_Full(queue))
    {
        if (queue->buffer[queue->tail] != NULL)
        {
            free(queue->buffer[queue->tail]);
            queue->buffer[queue->tail] = NULL;
        }
        queue->tail = (queue->tail + 1) % QUEUE_SIZE;
        queue->count--;
    }

    if (queue->buffer[queue->head] != NULL) {
        free(queue->buffer[queue->head]);
        queue->buffer[queue->head] = NULL;
    }

    queue->buffer[queue->head] = data;
    queue->head = (queue->head + 1) % QUEUE_SIZE;
    queue->count++;
    return true;
}

// Remove um elemento da fila
bool queueDequeuePopFirst(CircularQueue* queue)
{
    if (QueueisEmpty(queue)) return false;
    if (queue->buffer[queue->tail] != NULL)
    {
        free(queue->buffer[queue->tail]);
        queue->buffer[queue->tail] = NULL;
    }
    queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    queue->count--;
    return true;
}

// Deleta o último item da fila e libera sua memória
bool queueDequeuePopLast(CircularQueue* queue)
{
    if (QueueisEmpty(queue)) return false;

    uint8_t lastIndex = (queue->head == 0) ? (QUEUE_SIZE -1) : (queue->head -1);
    if (queue->buffer[lastIndex] != NULL)
    {
        free(queue->buffer[lastIndex]);
        queue->buffer[lastIndex] = NULL;
    }
    queue->head = lastIndex;
    queue->count--;
    return true;
}

// Espia o primeiro elemento da fila
bool queuePeekfirst(const CircularQueue* queue, operatingSystem* ptrData)
{
    if (QueueisEmpty(queue) || !queue->buffer[queue->tail]) return false;
    
    *ptrData = queue->buffer[queue->tail];
    return true;
}

// Espia o último elemento da fila
bool queuePeeklast(const CircularQueue* queue, operatingSystem* ptrData)
{
    if (QueueisEmpty(queue)) return false;
    
    uint8_t lastIndex = (queue->head == 0) ? (QUEUE_SIZE -1) : (queue->head -1);
    
    if (!queue->buffer[lastIndex]) return false;
    
    *ptrData = queue->buffer[lastIndex];
    return true;
}

// Espia o elemento do meio da fila
bool queuePeekmiddle(const CircularQueue* queue, operatingSystem* ptrData)
{
    if (QueueisEmpty(queue)) return false;
    
    uint8_t offset = queue->count / 2;
    uint8_t index = (queue->tail + offset) % QUEUE_SIZE;
    
    if (!queue->buffer[index]) return false;
    
    *ptrData = queue->buffer[index];
    return true;
}

#ifdef                    __cplusplus
extern                            "C" 
{
#                               endif
    void setup(void); void loop(void);
#ifdef                     __cplusplus
}
#                                endif
#                                endif // REGB_H
