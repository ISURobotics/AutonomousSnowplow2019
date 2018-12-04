#include <Servo.h>

Servo leftServo;
Servo rightServo;
//wire servos to make them go to the same direction

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(115200);
  leftServo.attach(9);
  rightServo.attach(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  int op = 0, intensity = 0, stopc = 0;
  if (Serial.available() > 2) {
    //read in the byte
    op = Serial.read();
    intensity = Serial.read();
    stopc = Serial.read();
  }

//  for (int i = 0; i < stopc; i++)
//  {
//    digitalWrite(LED_BUILTIN, HIGH);
//    delay(50);
//    digitalWrite(LED_BUILTIN, LOW);
//    delay(50);
//  }

  int power = map(intensity, 0, 255, 0, 500);
  //check for -1 flag
  if (stopc == 0x5) {
    digitalWrite(LED_BUILTIN, HIGH);
    if (op == 0x0) {
      //go straigth
      move_forward(power);
    } else if (op == 1) {
      turn_left(power);
    } else if (op == 2) {
      //go right
      turn_right(power);
    } else if (op == 3) {
      //stop
      stop_();
    }
  }
}
void move_forward(int power)
{
  leftServo.writeMicroseconds(1500 + power);
  rightServo.writeMicroseconds(1500 + power);
}
void turn_left(int power)
{
  leftServo.writeMicroseconds(1500 + power);
  rightServo.writeMicroseconds(1500 - power);
}
void turn_right(int power)
{
  leftServo.writeMicroseconds(1500 - power);
  rightServo.writeMicroseconds(1500 + power);
}
void move_backward(int power)
{
  leftServo.writeMicroseconds(1500 - power);
  rightServo.writeMicroseconds(1500 - power);
}
void stop_()
{
  leftServo.writeMicroseconds(1500);
  rightServo.writeMicroseconds(1500);
}
