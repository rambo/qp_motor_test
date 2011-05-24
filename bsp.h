#ifndef bsp_h
#define bsp_h

#include <avr/io.h>                                                 // AVR I/O

                                                 // Sys timer tick per seconds
#define BSP_TICKS_PER_SEC    1000

void BSP_init(void);

//////////////////////////////////////////////////////////////////////////////
// NOTE: The CPU clock frequency F_CPU is defined externally for each
// Arduino board

#endif                                                                // bsp_h


