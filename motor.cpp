#include "bsp.h"
#include "qp_port.h"
#include "motor.h"

Q_DEFINE_THIS_FILE


/**
 * Note to self: the QTimeEvt members must be initialized via initialization list or weird errors occur
 *
 * See: http://www.cprogramming.com/tutorial/initialization-lists-c++.html
 */
motor::motor() : QActive((QStateHandler)&motor::initial), pwm_timer(PWM_TIMEOUT_SIG), stall_timer(STALL_TIMEOUT_SIG)
{
    this->power = 10;
}

void motor::setup(uint8_t a1, uint8_t a2, uint8_t pulse)
{
    this->a1_pin = a1;
    this->a2_pin = a2;
    this->pulse_pin = pulse;
    pinMode(this->a1_pin, OUTPUT);
    digitalWrite(this->a1_pin, LOW);
    pinMode(this->a2_pin, OUTPUT);
    digitalWrite(this->a2_pin, LOW);
    pinMode(this->pulse_pin, INPUT);
    PCintPort::attachInterrupt(pulse_pin, &motor::pulse_handler, RISING, this);
}

void motor::pulse_handler(void* userData)
{
    //DEBUG_PRINT("Pulse INT");
    motor* me = (motor*)userData;
    me->postFIFO(Q_NEW(QEvent, PULSE_SIG));
}

QState motor::initial(motor *me, QEvent const *)
{
    //me->subscribe(PULSE_SIG);
    return Q_TRAN(&motor::stopped);
}

QState motor::stopped(motor *me, QEvent const *e)
{
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
        {
            // We need to do this here or the PWM stuff will play merry hell with the timer (alternatively add another level of hierarchy but that would be kinda ugly too)
            me->stall_timer.disarm();
            digitalWrite(me->a1_pin, LOW);
            digitalWrite(me->a2_pin, LOW);
            return Q_HANDLED();
        }
        case DRIVE_SIG:
        {
            me->power = ((drive_event *)e)->power;
            Q_ASSERT(me->power < 11);
            me->target_position += ((drive_event *)e)->amount;
            if (((drive_event *)e)->amount > 0)
            {
                me->direction = true;
            }
            else
            {
                me->direction = false;
            }
            me->pwm_transition_to_downtime_in = 0; // Clear the counter
            me->pwm_transition_to_uptime_in = (unsigned int)(MOTOR_PWM_FULL_TICKS * (10 - me->power) / 10); // This will evaluate to 0 at 10 power
            if (me->pwm_transition_to_uptime_in)
            {
                me->pwm_transition_to_downtime_in = MOTOR_PWM_FULL_TICKS - me->pwm_transition_to_uptime_in;
            }
            DEBUG_PRINT("DRIVE_SIG");
            Serial.print("me->power=");
            Serial.println(me->power, DEC);
            Serial.print("me->pwm_transition_to_downtime_in=");
            Serial.println(me->pwm_transition_to_downtime_in, DEC);
            Serial.print("me->pwm_transition_to_uptime_in=");
            Serial.println(me->pwm_transition_to_uptime_in, DEC);
            me->stall_timeout_adjusted = (unsigned int)(STALL_TIMEOUT_TICKS * (20 - me->power) / 10); // Evaluates to at STALL_TIMEOUT_TICKS 10 power
            Serial.print("me->stall_timeout_adjusted=");
            Serial.println(me->stall_timeout_adjusted, DEC);
            // We need to do this here or the PWM stuff will play merry hell with the timer (alternatively add another level of hierarchy but that would be kinda ugly too)
            me->stall_timer.postIn(me, me->stall_timeout_adjusted); // Adjust the stall timer up when power is below 100
            return Q_TRAN(&motor::driving);
        }
        // Track pulses from inertia etc
        case PULSE_SIG:
        {
            if (me->direction)
            {
                me->position++;
            }
            else
            {
                me->position--;
            }
            DEBUG_PRINT("PULSE_SIG");
            Serial.print("me->position=");
            Serial.println(me->position, DEC);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm::top);
}

QState motor::driving(motor *me, QEvent const *e)
{
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
        {
            //DEBUG_PRINT("Q_ENTRY_SIG");
            if (me->pwm_transition_to_downtime_in)
            {
                me->pwm_timer.postIn(me, me->pwm_transition_to_downtime_in);
            }
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
        case PWM_TIMEOUT_SIG:
        {
            //DEBUG_PRINT("PWM_TIMEOUT_SIG");
            return Q_TRAN(&motor::driving_downtime);
        }
        case STALL_TIMEOUT_SIG:
        {
            DEBUG_PRINT("STALL_TIMEOUT_SIG");
            motor_event *se;
            se = Q_NEW(motor_event, MOTOR_STALLED_SIG);
            // se->motor_id = TODO
            QF::publish(se);
            return Q_TRAN(&motor::stopped);
        }
        case DRIVE_SIG:
        {
            // PONDER: Either ignore it or what ??
            return Q_HANDLED();
        }
        case PULSE_SIG:
        {
            me->stall_timer.rearm(me->stall_timeout_adjusted);
            DEBUG_PRINT("PULSE_SIG");
            /*
            Serial.print("me->position=");
            Serial.println(me->position, DEC);
            Serial.print("me->target_position=");
            Serial.println(me->target_position, DEC);
            */
            if (me->direction)
            {
                me->position++;
                if (me->position >= me->target_position)
                {
                    DEBUG_PRINT("PULSE_SIG, sending done");
                    motor_event *de;
                    de = Q_NEW(motor_event, MOTOR_DONE_SIG);
                    // de->motor_id = TODO
                    QF::publish(de);
                    //DEBUG_PRINT("PULSE_SIG, transitioning");
                    return Q_TRAN(&motor::stopped);
                }
            }
            else
            {
                me->position--;
                if (me->position <= me->target_position)
                {
                    DEBUG_PRINT("PULSE_SIG, sending done");
                    motor_event *de;
                    de = Q_NEW(motor_event, MOTOR_DONE_SIG);
                    // de->motor_id = TODO
                    QF::publish(de);
                    //DEBUG_PRINT("PULSE_SIG, transitioning");
                    return Q_TRAN(&motor::stopped);
                }
            }
            //DEBUG_PRINT("PULSE_SIG, handled");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG:
        {
            //DEBUG_PRINT("Q_EXIT_SIG");
            digitalWrite(me->a1_pin, LOW);
            digitalWrite(me->a2_pin, LOW);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm::top);
}

QState motor::driving_downtime(motor *me, QEvent const *e)
{
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
        {
            //DEBUG_PRINT("Q_ENTRY_SIG");
            me->pwm_timer.postIn(me, me->pwm_transition_to_uptime_in);
            return Q_HANDLED();
        }
        case PWM_TIMEOUT_SIG:
        {
            //DEBUG_PRINT("PWM_TIMEOUT_SIG");
            return Q_TRAN(&motor::driving);
        }
        case Q_EXIT_SIG:
        {
            //DEBUG_PRINT("Q_EXIT_SIG");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm::top);
}


