import KinectPV2.*;
import processing.serial.*;

Serial myPort;
KinectPV2 kinect;
FaceData [] faceData;
boolean foundUsers = false;
int locX = -1;
int angle = 0;
int lastAngle = 0;
String val; // save the flag info
boolean firstContact = false; // since we're doing serial handshaking, we need to check if we heard frim the arduino
int threshold = 15;

void setup() {
  size(1024, 424);
  
  // setup contact with arduino
  String portName = Serial.list()[0]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 9600);
  myPort.bufferUntil('\n');
  
  // setup contact with Kinect
  kinect = new KinectPV2(this);
  //for face detection base on the infrared Img
  kinect.enableInfraredImg(true);
  kinect.enableFaceDetection(true);
  kinect.init();
}

void draw() {
  background(0);
  kinect.generateFaceData();
  
  //draw face information obtained by the color frame
  image(kinect.getInfraredImage(), 256, 0);
  
  foundUsers = false;
  locX = getFaceMapInfraredCenter();

  fill(255);
  textSize(16);
  text(frameRate, 50, 50);
  if(locX != -1){
    foundUsers = true;
    text("Your location on axis X is "+locX, 50, 90);
    //println("Angel is "+int(locX/512.0*180));
    //myPort.write(int(locX/512.0*180));
  }else{
    //myPort.write(0);
  }
  text("Found User: "+foundUsers, 50, 70);
}

void serialEvent(Serial myPort){
  val = myPort.readStringUntil('\n');
  if(val != null) {
    val = trim(val);
    //println(val);
    
    if(firstContact == false) {
      if(val.equals("A")) {
        myPort.clear();
        firstContact = true;
        myPort.write("A");
        println("contact");
      }
    }
    else {
       println(val);
       if(val.equals("B")) {
         if(locX != -1) { 
            angle = int(locX/512.0*180);
            if(Math.abs(angle - lastAngle) > threshold) {
              println("Angel is "+angle);
              myPort.write(angle);
              lastAngle = angle;
            }
          }else{
            myPort.write(lastAngle);
          }
       }
       
       myPort.write("A");
    }
  }
}

void mousePressed() {
  println(frameRate);
  ///saveFrame();
}

public int getFaceMapInfraredCenter() {
  ArrayList<FaceData> faceData =  kinect.getFaceData();
  for (int i = 0; i < faceData.size(); i++){
    FaceData faceD = faceData.get(i);
    if (faceD.isFaceTracked()) {
      KRectangle rectFace = faceD.getBoundingRectInfrared();
      return int(rectFace.getX() + rectFace.getWidth()/2);
    }
  }
  return -1;
}
