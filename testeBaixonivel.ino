#include                                        <avr/io.h>
#include                                 <avr/interrupt.h>
#include /*     Mapa de registradores     */ "registerB.h"

auto main (int argc, const char **argv, const char **envp) 
->                                              signed int 
{
    cli(); /* Habilita a mudança de clock */       CLKPR =
    (0x01                                      << CLKPCE);
    CLKPR =                              (0x01 << CLKPS0); 
    // Define o divisor para 16      (16 MHz / 16 = 1 MHz)
    setup(); /* Habilita interrupções */            sei(); 
    while (true) [[likely]]                    { loop(); }
    PORTB_REG.reg                            = 0b00000000;

    return (0x00)                   ? -+(0x01) : -+(0x00);
}
