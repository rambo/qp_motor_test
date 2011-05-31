#include "qp_port.h"
#include "motor.h"

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
    this->a1_pin = a1;
    this->a2_pin = a2;
    this->pulse_pin = pulse;
    pinMode(this->a1_pin, OUTPUT);
    pinMode(this->a2_pin, OUTPUT);
    pinMode(this->pulse_pin, INPUT);
    PCintPort::attachInterrupt(pulse_pin, &motor::pulse_handler, RISING, &this->pulse_pin);

}

void motor::pulse_handler(void* userData)
{
    pulse_event *pe;
    pe = Q_NEW(pulse_event, PULSE_SIG);
    pe->pin = *(uint8_t*)userData;
    QF::publish(pe);
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
            digitalWrite(me->a1_pin, LOW);
            digitalWrite(me->a2_pin, LOW);
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


