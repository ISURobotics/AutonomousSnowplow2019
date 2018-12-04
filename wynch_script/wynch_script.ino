#include <Servo.h>
//int servo_pin = 9;
Servo myservo;

void setup() {
  // put your setup code here, to run once:
  myservo.attach(10);

}

void loop() {
  // put your main code here, to run repeatedly:

  myservo.writeMicroseconds(2000);
  delay(1000);
  myservo.writeMicroseconds(1500);
  delay(1000);
}
