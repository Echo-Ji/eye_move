#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int lastPos = 0;
int pos = 0;    // variable to store the servo position
int angle = 0;

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  Serial.begin(9600); 
  establishContact();
}

void loop() {
  if (Serial.available()) {
    angle = Serial.read();
    if(angle != 'A'){
      for (pos = lastPos; pos <= angle; pos += 1) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
      lastPos = angle;
//      for (pos = angle; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//        myservo.write(pos);              // tell servo to go to position in variable 'pos'
//        delay(15);                       // waits 15ms for the servo to reach the position
//      }
      delay(100);
      Serial.println("B");  //this is the flag: I need data.
    }
  }else {
//    for (pos = lastPos; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//      myservo.write(pos);              // tell servo to go to position in variable 'pos'
//      delay(15);                       // waits 15ms for the servo to reach the position
//    }
//    lastPos = 0;
    Serial.println("B");
    delay(50); 
  }
}

void establishContact() {
  while(Serial.available() <= 0){
      Serial.println("A"); // this is the flag for first connection
      delay(300);  
  }
}
