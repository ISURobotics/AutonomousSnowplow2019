#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#define BNO055_SAMPLERATE_DELAY_MS (10)
Adafruit_BNO055 bno = Adafruit_BNO055();

#define ROLLING_AVG_NUM 15

int rolling_avg[ROLLING_AVG_NUM] = {0};

void setup() {
  Serial.begin(115200);
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  bno.setExtCrystalUse(true);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
}



void loop() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
//  imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  
  Serial.print("[");
  Serial.print(360-euler.x());
  Serial.println("]");


//  Serial.print("The Angular velocity is: ");
//  Serial.print("x: ");
//  Serial.print(gyro.x());
//  Serial.print(" y: ");
//  Serial.print(gyro.y());
//  Serial.print(" z: ");
//  Serial.println(gyro.z());

//  shift_array(rolling_avg, ROLLING_AVG_NUM);
//  rolling_avg[0] = gyro.x();
//  Serial.print("Avg: ");
//  Serial.println(get_avg(rolling_avg, ROLLING_AVG_NUM));
//  Serial.println(gyro.x());

  
  
  delay(BNO055_SAMPLERATE_DELAY_MS);
}

void shift_array(int * arr, int length_arr){

  for(int i = 0;i < length_arr;i++){
    
    arr[i+1] = arr[i];
    
  }
  
}

double get_avg(int * arr, int length_arr){

  double total = 0.0;

  for(int i = 0;i < length_arr;i++){

    total += arr[i];
    
  }

  return (total / (double)length_arr);
  
}
