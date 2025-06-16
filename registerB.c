#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "registerB.h"
#include "S_inC.h"

// Protótipos das funções
static void toggle(void);
static void analog(volatile uint8_t, volatile int);
static void status(void);

static volatile uint8_t pwm = 0;
static volatile int posineg = 1;
static void (*watchdog)(void) = NULL; // Era função inválida, agora é função nula

// Inicializa a tarefa
#ifdef NUM_TASKS
operatingSystem tasks[NUM_TASKS] = 
{
    { .funcSp = toggle, .interval_ms = 0x1F4, .counter = 0x00, .ok = false, 
      .padding = {0b00000000, 0b00000000, 0b00000000} // sem args
    },
    { .funcCp = analog, .interval_ms = 0x1F4, .counter = 0x00, .ok = false, 
      .padding = {0b00000001, 0b00000010, 0b00000000} // args
    },
    { .funcSp = status, .interval_ms = 0x64, .counter = 0x00, .ok = false, 
      .padding = {0b00000000, 0b00000000, 0b00000000} // sem args
    }
};
#else 
    #warning NUM_TASKS não definida... Isso pode resultar em erro.
#endif

// Caixa de status do sistema...
#if defined(LINE) && defined(COLUMN)
// status do sistema
static const char* monitor[LINE][COLUMN] = 
{
    {"Inicializando OS.\r\n",   "Configurando timer 1, do atmega328p. \n"},
    {"Abilitando flag no ciclo de interupição. \n",    "Chamando tarefa. \n"},
    {"Executando... \n",        "Encerando OS. \r\n"}
};

#elif defined(PORTB_REG) || defined(PORTC_REG) || defined(PORTD_REG)
    #warning Tudo ok, hardware mapeado via software. Cuidado isso pode falhar!!

void setup(void) 
{
    // Configura PB5 e PB0 como saída
    DDRB |= ((1 << PB5) | (1 << PB0));
    PORTB_REG.pb5 = 0;
    PORTB_REG.pb0 = 0;

    // Inicialização da Fila circular para manipulação de tarefas do OS
    InitQueue(&queueOS);

    // Inserindo tarefas na fila
    queueEnqueue(&queueOS, &tasks[0]);
    queueEnqueue(&queueOS, &tasks[1]);
    queueEnqueue(&queueOS, &tasks[2]);
    Serial_print(monitor[0][0]);

    // Configura Timer1 em modo CTC com prescaler 64
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (0 << CS12) | (1 << CS11) | (1 << CS10); // 64x prescaler, modo CTC
    OCR1A = 250; // 16MHz / 64 = 250kHz → 250 ciclos = 1 ms
    TIMSK1 |= (1 << OCIE1A); // Habilita interrupção por comparação A

    // --- Configura Timer2 para Fast PWM no pino OC2B ---
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20); // Fast PWM, não-invertido
    TCCR2B = (1 << CS21); // Prescaler = 8
    OCR2B = pwm;
    Serial_print(monitor[0][1]);
}

void loop(void) 
{
    for (int i = 0; i < NUM_TASKS; ++i) 
    {
        if (tasks[i].ok) 
        {
            tasks[i].ok = false;
            if (i == 0 && queueOS.buffer[i]->padding[0] == 0 && queueOS.buffer[i]->padding[1] < 1)
            {
                if (tasks[i].funcSp == NULL) 
                {
                    if (watchdog) watchdog();
                    Serial_print(monitor[2][1]);
                }
                else
                    queueOS.buffer[i]->funcSp();
            }
            else if (i == 1 && queueOS.buffer[i]->padding[0] != 0 && queueOS.buffer[i]->padding[1] > 1)
            {
                if (tasks[i].funcCp == NULL)
                {
                    if (watchdog) watchdog();
                    Serial_print(monitor[2][1]);
                }
                else
                    queueOS.buffer[i]->funcCp(pwm, posineg);
            }
            else
            {
                if (tasks[i].funcSp == NULL)
                {
                    if (watchdog) watchdog();
                    Serial_print(monitor[2][1]);
                }
                else
                    queueOS.buffer[i]->funcSp();
            }
        }
    }
}

// Interrupção de Timer1 a cada 1ms
ISR(TIMER1_COMPA_vect) 
{
    for (int i = 0; i < NUM_TASKS; ++i)
    {
        tasks[i].counter++;
        if (tasks[i].counter >= tasks[i].interval_ms) 
        { 
            tasks[i].counter = 0;
            tasks[i].ok = true; 
            // Exibe status
            Serial_print(monitor[1][0]);
            Serial_print(monitor[1][1]);
            Serial_print(monitor[2][0]);
        }
    }
}

// Alterna o estado de PB5 (LED)
static void toggle(void)
{
    PORTB_REG.pb5 ^= 0x01;
}

static void analog(volatile uint8_t i, volatile int y)
{
    i += y;
    if (i == 0xff || i == 0x00) { y = -y; }
    OCR2B = i;
}

static void status(void)
{
    Serial_print("Sistema operacional em execução\n");
}

#else
    #error Registrador do microcontrolador não configurado... Mapeamento via software falhou!!
#endif
