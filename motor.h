#ifndef motor_h
#define motor_h
#include <WProgram.h> 
#include "qp_port.h"
#include "bsp.h"
// This might not be needed, or even desired but it should make debugging simpler
#define DISABLE_PCINT_MULTI_SERVICE
#include <PinChangeInt_userData.h>

#ifndef STALL_TIMEOUT_TICKS
#define STALL_TIMEOUT_TICKS (unsigned int)(BSP_TICKS_PER_SEC / 2)
#endif


enum MotorSignals {
   MOTOR_DONE_SIG = Q_USER_SIG,        // published by motor
   MOTOR_STALLED_SIG,        // published by motor
   TERMINATE_SIG,             // published by BSP to terminate the application
   MAX_PUB_SIG,                                   // the last published signal

   PULSE_SIG,                      // posted directly to motor, done by the interrupt handler attached in setup()
   DRIVE_SIG,                      // posted directly to motor, tell it to drive X pulses
   PWM_TIMEOUT_SIG,
   STALL_TIMEOUT_SIG,
   MAX_SIG                                                  // the last signal
};

/**
 * pub/sub events published by the motor instances
 */
struct motor_event : public QEvent
{
    uint8_t motor_id;
};

/**
 * events posted directly to the motor telling it to drive
 */
struct drive_event : public QEvent
{
    int amount; //contains direction too.
};

class motor : public QActive
{
    public:
        motor();
        void setup(uint8_t a1, uint8_t a2, uint8_t pulse);
        static void pulse_handler(void* me_ptr);

    protected:
        uint8_t a1_pin;
        uint8_t a2_pin;
        uint8_t pulse_pin;
        boolean direction;
        int position;
        int target_position;
        QTimeEvt pwm_timer;
        QTimeEvt stall_timer;

        static QState initial (motor *me, QEvent const *e);
        static QState stopped (motor *me, QEvent const *e);
        static QState driving (motor *me, QEvent const *e);

        static void pulse_handler();
};


#endif
