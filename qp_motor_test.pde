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
