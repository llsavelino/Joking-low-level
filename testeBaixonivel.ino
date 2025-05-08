#include        <avr/io.h>
#include <avr/interrupt.h>
#include     "registerB.h"

auto main (int argc, const char **argv, const char **envp) 
noexcept(false)                              -> signed int 
{
    cli();
    CLKPR = (0x01 << CLKPCE); /* Habilita a mudança de clock */
    CLKPR = (0x01 << CLKPS0); // Define o divisor para 16 (16 MHz / 16 = 1 MHz)
    setup(); sei(); // Habilita interrupções
    while (true) [[likely]] { loop(); }
    PORTB_REG.reg = 0b00000000;

    return 0;
}
