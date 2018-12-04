#include <Servo.h> 

Servo myservo;

void setup() 
{ 
  myservo.attach(9);
  myservo.writeMicroseconds(1500);  // set servo to mid-point
} 

void loop(){
 myservo.writeMicroseconds(2000);
  delay(10000);
  myservo.writeMicroseconds(1500);
  delay(1000);
  myservo.writeMicroseconds(1000);
  delay(10000);
  myservo.writeMicroseconds(1500);
  delay(1000);
}
