#include "qp_port.h"
#include "motor.h"
//#include "PinChangeInt_userData.h"
//#include <PinChangeInt_userData.h>

//Q_DEFINE_THIS_FILE


/**
 * Note to self: the QTimeEvt members must be initialized via initialization list or weird errors occur
 *
 * See: http://www.cprogramming.com/tutorial/initialization-lists-c++.html
 */
motor::motor() : QActive((QStateHandler)&motor::initial), pwm_timer(PWM_TIMEOUT_SIG) , stall_timer(STALL_TIMEOUT_SIG)
{
}

void motor::setup(uint8_t a1, uint8_t a2, uint8_t pulse)
{
    a1_pin = a1;
    a2_pin = a2;
    pulse_pin = pulse;
    pinMode(a1_pin, OUTPUT);
    pinMode(a2_pin, OUTPUT);
    pinMode(pulse_pin, INPUT);
    /**
     * Passing pointers to member functions is pratically verboten
     *
     * See: http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.1
     *
    PCintPort::attachInterrupt(pulse_pin, &pulse_handler, RISING);
     */

}

QState motor::initial(motor *me, QEvent const *)
{
    return Q_TRAN(&motor::stopped);
}

QState motor::stopped(motor *me, QEvent const *e)
{
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
        {
            me->a1_pin = LOW;
            me->a2_pin = LOW;
        }
        case PULSE_SIG:
        {
            if (((pulse_event *)e)->pin == me->pulse_pin)
            {
                if (me->direction)
                {
                    me->position++;
                }
                else
                {
                    me->position--;
                }
            }
        }
        default:
        {
            return Q_HANDLED();
        }
    }

    return Q_SUPER(&QHsm::top);
}


