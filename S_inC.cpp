#include                 "S_inC.h"
#include               <Arduino.h>
void Serial_print(const char* msg)
{   Serial.print(msg + '\0');    }
