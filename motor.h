#ifndef motor_h
#define motor_h
#include <WProgram.h> 
#include <PinChangeInt_userData.h>


//#include <stdint.h>
#include "qp_port.h"
#include "bsp.h"

enum MotorSignals {
   MOTOR_DONE_SIG = Q_USER_SIG,        // published by motor
   STALLED_SIG,        // published by motor
   PULSE_SIG,                      // posted globally since binding ISRs to member functions is just-about-impossible
   TERMINATE_SIG,             // published by BSP to terminate the application
   MAX_PUB_SIG,                                   // the last published signal

   DRIVE_SIG,                      // posted directly to motor, tell it to drive X pulses
   PWM_TIMEOUT_SIG,
   STALL_TIMEOUT_SIG,
   MAX_SIG                                                  // the last signal
};

struct pulse_event : public QEvent
{
    uint8_t pin;
};

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
