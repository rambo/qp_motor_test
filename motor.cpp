#include "bsp.h"
#include "qp_port.h"
#include "motor.h"

//Q_DEFINE_THIS_FILE


/**
 * Note to self: the QTimeEvt members must be initialized via initialization list or weird errors occur
 *
 * See: http://www.cprogramming.com/tutorial/initialization-lists-c++.html
 */
motor::motor() : QActive((QStateHandler)&motor::initial), pwm_timer(PWM_TIMEOUT_SIG), stall_timer(STALL_TIMEOUT_SIG)
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
    me->subscribe(PULSE_SIG);
    DEBUG_PRINT("hello world!");
    return Q_TRAN(&motor::stopped);
}

QState motor::stopped(motor *me, QEvent const *e)
{
    DEBUG_PRINT("hello world!");
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
        {
            DEBUG_PRINT("Q_ENTRY_SIG");
            digitalWrite(me->a1_pin, LOW);
            digitalWrite(me->a2_pin, LOW);
            return Q_HANDLED();
        }
        case DRIVE_SIG:
        {
            DEBUG_PRINT("DRIVE_SIG");
            me->target_position += ((drive_event *)e)->amount;
            if (((drive_event *)e)->amount > 0)
            {
                me->direction = true;
            }
            else
            {
                me->direction = false;
            }
            DEBUG_PRINT("DRIVE_SIG, transitioning");
            return Q_TRAN(&motor::driving);
        }
        // Track pulses from inertia etc
        case PULSE_SIG:
        {
            DEBUG_PRINT("PULSE_SIG");
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
            return Q_HANDLED();
        }
    }

    return Q_SUPER(&QHsm::top);
}

QState motor::driving(motor *me, QEvent const *e)
{
    DEBUG_PRINT("Hello world");
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
        {
            DEBUG_PRINT("Q_ENTRY_SIG");
            if (me->direction)
            {
                digitalWrite(me->a1_pin, HIGH);
                digitalWrite(me->a2_pin, LOW);
            }
            else
            {
                digitalWrite(me->a1_pin, LOW);
                digitalWrite(me->a2_pin, HIGH);
            }
            return Q_HANDLED();
        }
        case DRIVE_SIG:
        {
            DEBUG_PRINT("DRIVE_SIG");
            // PONDER: Either ignore it or what ??
            return Q_HANDLED();
        }
        case PULSE_SIG:
        {
            DEBUG_PRINT("PULSE_SIG");
            if (((pulse_event *)e)->pin == me->pulse_pin)
            {
                if (me->direction)
                {
                    me->position++;
                    if (me->position >= me->position)
                    {
                        DEBUG_PRINT("PULSE_SIG, sending done");
                        QF::publish(Q_NEW(QEvent, MOTOR_DONE_SIG));
                        return Q_TRAN(&motor::stopped);
                    }
                }
                else
                {
                    me->position--;
                    if (me->position <= me->position)
                    {
                        DEBUG_PRINT("PULSE_SIG, sending done");
                        QF::publish(Q_NEW(QEvent, MOTOR_DONE_SIG));
                        return Q_TRAN(&motor::stopped);
                    }
                }
            }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG:
        {
            DEBUG_PRINT("Q_EXIT_SIG");
            digitalWrite(me->a1_pin, LOW);
            digitalWrite(me->a2_pin, LOW);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm::top);
}

