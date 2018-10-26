#include <Servo.h>

//servo 1 is the left one
//servo 2 is the right one
Servo myservo1;
Servo myservo2;
//wire servos to make them go to the same direction

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(9600);
  myservo1.attach(9);
  myservo2.attach(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  int op=0, intensity=0, stopc=0;
  if(Serial.available() > 2){
     //read in the byte
    op = Serial.read();
    intensity = Serial.read();
    stopc = Serial.read();

  }

  for(int i = 0;i<stopc;i++)
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
}
      
      int power = map(intensity,0,255,0,500);
      //check for -1 flag
    if (stopc == 0x5){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(3000);
      if(op == 0x0){
        //go straigth
        myservo1.writeMicroseconds(1500 + power);
        myservo2.writeMicroseconds(1500 + power);
        } else if (op==1){
        //go left
        myservo1.writeMicroseconds(1500 - power);
        myservo2.writeMicroseconds(1500 + power);
        } else if (op == 2){
        //go right
        myservo1.writeMicroseconds(1500 + power);
        myservo2.writeMicroseconds(1500 - power);
        } else if (op == 3){
        //stop
        myservo1.writeMicroseconds(1500);
        myservo2.writeMicroseconds(1500);       
        }
      }
}
 
