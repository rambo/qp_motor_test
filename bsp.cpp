//////////////////////////////////////////////////////////////////////////////
// Product: Board Support Package for Arduino UNO
// Last Updated for Version: 4.1.06
// Date of the Last Update:  Jan 27, 2011
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2011 Quantum Leaps, LLC. All rights reserved.
//
// This software may be distributed and modified under the terms of the GNU
// General Public License version 2 (GPL) as published by the Free Software
// Foundation and appearing in the file GPL.TXT included in the packaging of
// this file. Please note that GPL Section 2[b] requires that all works based
// on this software must also be made publicly available under the terms of
// the GPL ("Copyleft").
//
// Alternatively, this software may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GPL and are specifically designed for licensees interested in
// retaining the proprietary status of their code.
//
// Contact information:
// Quantum Leaps Web site:  http://www.quantum-leaps.com
// e-mail:                  info@quantum-leaps.com
//////////////////////////////////////////////////////////////////////////////
#include "qp_port.h"
#include "bsp.h"
#include <Wprogram.h>



Q_DEFINE_THIS_FILE

//#define SAVE_POWER

#define USER_LED_ON()      (PORTB |= (1 << (5)))
#define USER_LED_OFF()     (PORTB &= ~(1 << (5)))
#define USER_LED_TOGGLE()  (PORTB ^= (1 << (5)))

#define TICK_DIVIDER       ((F_CPU / BSP_TICKS_PER_SEC / 1024) - 1)

#if TICK_DIVIDER > 255
#   error BSP_TICKS_PER_SEC too small
#elif TICK_DIVIDER < 2
#   error BSP_TICKS_PER_SEC too large
#endif

// ISRs ----------------------------------------------------------------------
ISR(TIMER2_COMPA_vect) {
    // No need to clear the interrupt source since the Timer2 compare
    // interrupt is automatically cleard in hardware when the ISR runs.
    Serial.println("Tick");
    QF::tick();                               // process all armed time events
}

//............................................................................
void BSP_init(void) {
    DDRB  = 0xFF;                     // All PORTB pins are outputs (user LED)
    PORTB = 0x00;                                        // drive all pins low

    Serial.begin(115200);
    Serial.println("Start");


}
//............................................................................
void QF::onStartup(void) {
          // set Timer2 in CTC mode, 1/1024 prescaler, start the timer ticking
    TCCR2A = ((1 << WGM21) | (0 << WGM20));
    TCCR2B = (( 1 << CS22 ) | ( 1 << CS21 ) | ( 1 << CS20 ));        // 1/2^10
    ASSR &= ~(1<<AS2);
    TIMSK2 = (1 << OCIE2A);                 // Enable TIMER2 compare Interrupt
    TCNT2 = 0;
    OCR2A = TICK_DIVIDER;     // must be loaded last for Atmega168 and friends
}
//............................................................................
void QF::onCleanup(void) {
}
//............................................................................
void QF::onIdle(QF_INT_KEY_TYPE key) {
    DEBUG_PRINT("Hello World!");

    USER_LED_ON();     // toggle the User LED on Arduino on and off, see NOTE1
    USER_LED_OFF();

#ifdef SAVE_POWER

    SMCR = (0 << SM0) | (1 << SE);  // idle sleep mode, adjust to your project

    // never separate the following two assembly instructions, see NOTE2
    __asm__ __volatile__ ("sei" "\n\t" :: );
    __asm__ __volatile__ ("sleep" "\n\t" :: );

    SMCR = 0;                                              // clear the SE bit

#else
    QF_INT_UNLOCK(key);
#endif
}

//............................................................................
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    /*
    // Copied from http://www.arduino.cc/en/Reference/PROGMEM
    char buffer[20];
    strcpy_P(buffer, (char*)pgm_read_word(file)); // Necessary casts and dereferencing, just copy. 
    */
    Serial.print(millis()); 
    Serial.print(": ASSERT "); 
    Serial.print(" in "); 
    //Serial.print(buffer); 
    Serial.print(":"); 
    Serial.println(line, DEC); 
    cli();                                              // lock all interrupts
    USER_LED_ON();                                  // User LED permanently ON
    while (true); // loop infinetely for now to avoid spamming
    asm volatile ("jmp 0x0000");    // perform a software reset of the Arduino
}

//////////////////////////////////////////////////////////////////////////////
// NOTE1:
// The Arduino's User LED is used to visualize the idle loop activity.
// The brightness of the LED is proportional to the frequency of invcations
// of the idle loop. Please note that the LED is toggled with interrupts
// locked, so no interrupt execution time contributes to the brightness of
// the User LED.
//
// NOTE2:
// The QF_onIdle() callback is called with interrupts *locked* to prevent
// a race condtion of posting a new event from an interrupt while the
// system is already committed to go to sleep. The only *safe* way of
// going to sleep mode is to do it ATOMICALLY with enabling interrupts.
// As described in the "AVR Datasheet" in Section "Reset and Interrupt
// Handling", when using the SEI instruction to enable interrupts, the
// instruction following SEI will be executed before any pending interrupts.
// As the Datasheet shows in the assembly example, the pair of instructions
//     SEI       ; enable interrupts
//     SLEEP     ; go to the sleep mode
// executes ATOMICALLY, and so *no* interrupt can be serviced between these
// instructins. You should NEVER separate these two lines.
//
