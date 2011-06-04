#include "qp_port.h"
#include "bsp.h"
#include "motor.h"
// If this is not here we get a compile error on i2c_accelerometer.h (WTF!)
#define DISABLE_PCINT_MULTI_SERVICE
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

    Serial.print("MOTOR_DONE_SIG=");
    Serial.println(MOTOR_DONE_SIG, DEC);
    
    Serial.print("STALLED_SIG=");    
    Serial.println(STALLED_SIG, DEC);    
    
    Serial.print("PULSE_SIG=");   
    Serial.println(PULSE_SIG, DEC);
    
    Serial.print("TERMINATE_SIG=");
    Serial.println(TERMINATE_SIG, DEC);
    
    Serial.print("MAX_PUB_SIG=");
    Serial.println(MAX_PUB_SIG, DEC);
    
    Serial.print("DRIVE_SIG=");   
    Serial.println(DRIVE_SIG, DEC);
    
    Serial.print("PWM_TIMEOUT_SIG=");
    Serial.println(PWM_TIMEOUT_SIG, DEC);
    
    Serial.print("STALL_TIMEOUT_SIG=");
    Serial.println(STALL_TIMEOUT_SIG, DEC);
    
    Serial.print("MAX_SIG=");
    Serial.println(MAX_SIG, DEC);

    QF::init();       // initialize the framework and the underlying RT kernel
    QF::poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
    QF::psInit(l_subscrSto, Q_DIM(l_subscrSto));     // init publish-subscribe

    motors[0].setup(2,3,4);
    motors[0].start(1, l_motorQueueSto[0], Q_DIM(l_motorQueueSto[0]));

    Serial.println("drive_event *de;");
    drive_event *de;
    // This triggers assert, apparently we run out of pool space, which should not be possible unless our pool init is messed up.
    Serial.println("de = Q_NEW(drive_event, DRIVE_SIG);");
    de = Q_NEW(drive_event, DRIVE_SIG);
    de->amount = -100;
    Serial.println("motors[0].postFIFO(de);");
    motors[0].postFIFO(de);

}







/*
// It seems this is required by PinChangeInt 
#include <WProgram.h>
#define DISABLE_PCINT_MULTI_SERVICE
#include <PinChangeInt.h>

void wheel_encoder_pulse_hi()
{
    Serial.print("w RISING pulse @");
    Serial.println(micros(), DEC);
}

void wheel_encoder_pulse_lo()
{
    Serial.print("w FALLING pulse @");
    Serial.println(micros(), DEC);
}

void ir_rec_hi()
{
    Serial.print("IR RISING pulse @");
    Serial.println(micros(), DEC);
}

void ir_rec_lo()
{
    Serial.print("IR FALLING pulse @");
    Serial.println(micros(), DEC);
}


void setup()
{
    Serial.begin(115200);  // start serial for output
    Serial.flush();
    Serial.println("Booting");

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

    Serial.println("Booted");
}


int i;
void loop()
{
    delay(1000);
    i++;
    Serial.print("i=");
    Serial.println(i, DEC);
    if (!(i % 2))
    {
        Serial.println("Foo");
        digitalWrite(2, HIGH);
        digitalWrite(3, LOW);
    }
    else
    {
        Serial.println("Bar");
        digitalWrite(2, LOW);
        digitalWrite(3, HIGH);
    }
    delay(3000);
}
*/
