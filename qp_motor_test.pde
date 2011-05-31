#include "qp_port.h"
#include "bsp.h"
#include "motor.h"
// If this is not here we get a compile error on i2c_accelerometer.h (WTF!)
#define DISABLE_PCINT_MULTI_SERVICE
#include <PinChangeInt_userData.h>



enum { N_MOTORS = 2 };                                // number of philosophers
static motor motors[N_MOTORS];
static QSubscrList   l_subscrSto[MAX_PUB_SIG];
static QEvent l_smlPoolSto[2*N_MOTORS];   // storage for the small event pool
static QEvent const *l_motorQueueSto[N_MOTORS][N_MOTORS];


//extern QActive * const AO_Motors[N_MOTORS];     // "opaque" pointers to Philo AO

void setup()
{
    
    BSP_init();                                          // initialize the BSP
    QF::init();       // initialize the framework and the underlying RT kernel
    QF::poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
    QF::psInit(l_subscrSto, Q_DIM(l_subscrSto));     // init publish-subscribe

    motors[0].setup(2,3,4);
    motors[0].start(1, l_motorQueueSto[0], Q_DIM(l_motorQueueSto[0]));

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
