#include "qp_port.h"
#include "bsp.h"
#include "motor.h"
// If this is not here we get a compile error on motor.cpp (WTF! it is included in motor.h)
#include <PinChangeInt_userData.h>



enum { N_MOTORS = 2 };                                // number of philosophers
static motor motors[N_MOTORS];
static QEvent const *l_smlPoolSto[4*N_MOTORS];
static QEvent const *l_motorQueueSto[N_MOTORS][4*N_MOTORS];
static QSubscrList   l_subscrSto[MAX_PUB_SIG];


//extern QActive * const AO_Motors[N_MOTORS];     // "opaque" pointers to Philo AO

void setup()
{
    BSP_init();                                          // initialize the BSP
    Serial.print(F("MOTOR_PWM_FULL_TICKS="));
    Serial.println(MOTOR_PWM_FULL_TICKS, DEC);
    Serial.print(F("STALL_TIMEOUT_TICKS="));
    Serial.println(STALL_TIMEOUT_TICKS, DEC);
    Serial.print(F("sizeof(drive_event)="));
    Serial.println(sizeof(drive_event), DEC);

    QF::init();       // initialize the framework and the underlying RT kernel
    QF::poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(drive_event));
    QF::psInit(l_subscrSto, Q_DIM(l_subscrSto));     // init publish-subscribe

    motors[0].setup(2,3,4);
    // Q_DIM(l_motorQueueSto[0]) I have a nagging feeling this should be something else (probably related to the size of the drive_event)
    motors[0].start(1, l_motorQueueSto[0], Q_DIM(l_motorQueueSto[0]));

    /*
    drive_event *de;
    de = Q_NEW(drive_event, DRIVE_SIG);
    de->amount = -100;
    motors[0].postFIFO(de);
    */
    drive_event *de;
    de = Q_NEW(drive_event, DRIVE_SIG);
    de->amount = -100;
    de->power = 2;
    motors[0].postFIFO(de);

}







/*
// It seems this is required by PinChangeInt 
#include <WProgram.h>
#define DISABLE_PCINT_MULTI_SERVICE
#include <PinChangeInt.h>

void wheel_encoder_pulse_hi()
{
    Serial.print(F("w RISING pulse @"));
    Serial.println(micros(), DEC);
}

void wheel_encoder_pulse_lo()
{
    Serial.print(F("w FALLING pulse @"));
    Serial.println(micros(), DEC);
}

void ir_rec_hi()
{
    Serial.print(F("IR RISING pulse @"));
    Serial.println(micros(), DEC);
}

void ir_rec_lo()
{
    Serial.print(F("IR FALLING pulse @"));
    Serial.println(micros(), DEC);
}


void setup()
{
    Serial.begin(115200);  // start serial for output
    Serial.flush();
    Serial.println(F("Booting"));

    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);

    pinMode(4, INPUT);
    PCintPort::attachInterrupt(4, &wheel_encoder_pulse_hi, RISING);
    PCintPort::attachInterrupt(4, &wheel_encoder_pulse_lo, FALLING);

    pinMode(8, INPUT);
    PCintPort::attachInterrupt(8, &ir_rec_hi, RISING);
    PCintPort::attachInterrupt(8, &ir_rec_lo, FALLING);

    Serial.println(F("Booted"));
}


int i;
void loop()
{
    delay(1000);
    i++;
    Serial.print(F("i="));
    Serial.println(i, DEC);
    if (!(i % 2))
    {
        Serial.println(F("Foo"));
        digitalWrite(2, HIGH);
        digitalWrite(3, LOW);
    }
    else
    {
        Serial.println(F("Bar"));
        digitalWrite(2, LOW);
        digitalWrite(3, HIGH);
    }
    delay(3000);
}
*/
