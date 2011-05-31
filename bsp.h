#ifndef bsp_h
#define bsp_h

#include <avr/io.h>                                                 // AVR I/O

#define DEBUG_PRINT(str) \
    Serial.print(millis()); \
    Serial.print(": "); \
    Serial.print(__FUNCTION__); \
    Serial.print("() in "); \
    Serial.print(__FILE__); \
    Serial.print(':'); \
    Serial.print(__LINE__); \
    Serial.print(' '); \
    Serial.println(str);

//    Serial.print(__FUNCDNAME__); \

                                                 // Sys timer tick per seconds
#define BSP_TICKS_PER_SEC    500

void BSP_init(void);

//////////////////////////////////////////////////////////////////////////////
// NOTE: The CPU clock frequency F_CPU is defined externally for each
// Arduino board

#endif                                                                // bsp_h


