#include "WifiPort2.h"

#include <Servo.h>

 

//TX VARIABLES

int b1Pin;

int b2Pin;

int b3Pin;

int b4Pin;

int JPin;

int XPin;

int YPin;

 

//RX VARIABLES

//Right Motor

int Rena1; // Arduino pin connected to Enable1 of H-Bridge

int Rdrive1A;// Arduino pin connected to In1 of H-Bridge

int Rdrive2A;// Arduino pin connected to In2 of H-Bridge

 

//Left Motor

int Lena1; // Arduino pin connected to Enable1 of H-Bridge

int Ldrive1A;// Arduino pin connected to In1 of H-Bridge

int Ldrive2A; // Arduino pin connected to In2 of H-Bridge

 

//Stepper Motors

bool initPos = 1;

int clawAngle = 20;

int armAngle = 70;

int clawChange = clawAngle;

int armChange = armAngle;

Servo Claw;

Servo Arm;

 

// A structure, similar to our servo and stepper motors, but this one conatins variables to be transmitted

// Any variable you want to transmit/recieve must be initalized in the DataPacket structure

struct DataPacket {

 

  int AnalogCheck;  //an initial check to show successful transmission

  int button1;

  int button2;

  int button3;

  int button4;

  int joystickX;

  int joystickY;  

  int joystickButton;                //YOU should wire up a simple resistor (220 ohm) circuit and manually probe it with a wire connected to A0

 

};

 

DataPacket data;//data.AnalogCheck

 

//gloabl vars are outside datapacket

 

WifiPort<DataPacket> WifiSerial;

 

void setup() {

  //DONT USE PIN13 FOR ANY SENSOR OR ACTUATORS

 

  Serial.begin(115200);  //preferred transmission rate for Arduino UNO R4

  //you need to update in your serial monitor

 

  //WifiSerial.begin("PairAP_CBGA41", "triplesr", WifiPortType::Transmitter);

  WifiSerial.begin("PairAP_CBGA41", "triplesr", WifiPortType::Receiver);

  //WifiSerial.begin("PairAP_CBGA41", "triplesr", WifiPortType::Emulator); // one board to rule them all debugging

  //meant to emulate both your controller and colebot on a single R3

  //8 Character, no special characters, no spaces password

 

  //Set Up for tx and rx

  if (WifiSerial.getPortType() == WifiPortType::Transmitter || WifiSerial.getPortType() == WifiPortType::Emulator){

    b1Pin = 3;

    b2Pin = 4;

    b3Pin = 5;

    b4Pin = 6;

    JPin = 2;

    XPin = A0;

    YPin = A1;

    pinMode(b1Pin,INPUT_PULLUP);//Button1

    pinMode(b2Pin,INPUT_PULLUP);//Button2

    pinMode(b3Pin,INPUT_PULLUP);//Button3

    pinMode(b4Pin,INPUT_PULLUP);//Button4

    pinMode(XPin, INPUT);//JoystickX

    pinMode(YPin, INPUT);//JoystickY

    pinMode(JPin,INPUT_PULLUP);//JoystickButton

  }

 

  if ((WifiSerial.getPortType() == WifiPortType::Receiver || WifiSerial.getPortType() == WifiPortType::Emulator)){

    //RECIEVER BOARD

 

    //Right Motor

    Rena1 = 3; // Arduino pin connected to Enable1 of H-Bridge

    Rdrive1A = 2; // Arduino pin connected to In1 of H-Bridge

    Rdrive2A = 4; // Arduino pin connected to In2 of H-Bridge

 

    //Left Motor

    Lena1 = 6; // Arduino pin connected to Enable1 of H-Bridge

    Ldrive1A = 5; // Arduino pin connected to In1 of H-Bridge

    Ldrive2A = 7; // Arduino pin connected to In2 of H-Bridge

 

    Claw.attach(A1);

    Arm.attach(A0);

    Claw.write(clawAngle);

    Arm.write(armAngle);

    delay(100);

 

    //Pin Moding it

    pinMode(Rena1,OUTPUT);

    pinMode(Rdrive1A,OUTPUT);

    pinMode(Rdrive2A,OUTPUT);

    pinMode(Lena1,OUTPUT); // set digital pin modes to output

    pinMode(Ldrive1A,OUTPUT);

    pinMode(Ldrive2A,OUTPUT);

  }

}

 

 

 

void loop() {

 

  if (WifiSerial.getPortType() == WifiPortType::Transmitter || WifiSerial.getPortType() == WifiPortType::Emulator) {

    WifiSerial.autoReconnect();//try and connect

 

    //Tx stuff below

    dataCheck();

    //Tx stuff above

    data.button1=digitalRead(3);

    data.button2=digitalRead(4);

    data.button3=digitalRead(5);

    data.button4=digitalRead(6);

    data.joystickX=analogRead(A0);

    data.joystickY=analogRead(A1);

    data.joystickButton=digitalRead(2);




    if (!WifiSerial.sendData(data))//check and see if connection is established and data is sent

      Serial.println("Wifi Send Problem");//oh no it didn't send --> it iwll try and re-connect at the start of the loop

 

  }

 

 

  if ((WifiSerial.getPortType() == WifiPortType::Receiver || WifiSerial.getPortType() == WifiPortType::Emulator) && WifiSerial.checkForData()) {

 

    data = WifiSerial.getData();//received and unpack data structure

 

    //all Rx stuff below

    dataCheck();

 

    //Skid Steering Stuff

    const int fwdSpeed = map(data.joystickX, 540,1023,0,255); //Find proportional value of 1028 as percentage of 255 for analogWrite

    const int bckSpeed = map(data.joystickX, 480,0,0,255);

 

    const int rSpeed = map(data.joystickY, 540,1023,0,255); //Same idea but for turning

    const int lSpeed = map(data.joystickY, 480,0,0,255);

 

    if(data.joystickX > 540){   //TURNING LEFT

      digitalWrite(Rdrive1A, LOW);  

      digitalWrite(Rdrive2A, HIGH);

      digitalWrite(Ldrive1A, LOW);  

      digitalWrite(Ldrive2A, HIGH);

      Serial.println(fwdSpeed);

      analogWrite(Rena1, fwdSpeed);

      analogWrite(Lena1, fwdSpeed);

    }

    else if (data.joystickX < 480){ //TURNING RIGHT

      digitalWrite(Rdrive1A, HIGH);  

      digitalWrite(Rdrive2A, LOW);

      digitalWrite(Ldrive1A, HIGH);  

      digitalWrite(Ldrive2A, LOW);

      analogWrite(Rena1, bckSpeed);

      analogWrite(Lena1, bckSpeed);

    }

    else if (data.joystickY > 540){ //FORWARDS

      digitalWrite(Rdrive1A, HIGH);  

      digitalWrite(Rdrive2A, LOW);

      digitalWrite(Ldrive1A, LOW);  

      digitalWrite(Ldrive2A, HIGH);

      analogWrite(Rena1, rSpeed);

      analogWrite(Lena1, rSpeed);

    }

    else if (data.joystickY < 480){ //BACKWARDS

      digitalWrite(Rdrive1A, LOW);  

      digitalWrite(Rdrive2A, HIGH);

      digitalWrite(Ldrive1A, HIGH);  

      digitalWrite(Ldrive2A, LOW);

      analogWrite(Rena1, lSpeed);

      analogWrite(Lena1, lSpeed);

    }

    else { //No movement when joystick centered

      digitalWrite(Rdrive1A, LOW);  

      digitalWrite(Rdrive2A, LOW);

      digitalWrite(Ldrive1A, LOW);  

      digitalWrite(Ldrive2A, LOW);

      analogWrite(Rena1, 0);

      analogWrite(Lena1, 0);

    }

 

    //Stepper Control

    if(data.button1 == LOW){ // open claw

      clawAngle = clawAngle -5;

    }

    if(data.button2 == LOW){ // close claw

      clawAngle = clawAngle + 5;

    }

 

    if(data.button3 == LOW){ // lower arm

      armAngle = armAngle - 5;

    }

    if(data.button4 == LOW){ // raise arm

      armAngle = armAngle + 5;

      Serial.println(armAngle);

    }

 

    clawAngle = constrain(clawAngle, 10, 55); //beautiful constraint functions

    armAngle = constrain(armAngle, 50, 125);

    if(clawAngle != clawChange){ //claw movement and limits

      Claw.write(clawAngle);

      clawChange = clawAngle;

      delay(15);

    }

    if(armAngle != armChange){ //arm movement and limits

      Arm.write(armAngle);

      armChange = armAngle;

      delay(15);

    }

 

      //gripper range: 0-45, 10 is safe start - must make manually

      //arm range: 20-110, 70 is safe start - must make manually

    //all RX stuff above

 

  }

 

  delay(100); // update delay after you get it working to be a smaller number like 10ms to account for WiFi transmission overhead

}

 

void dataCheck(){

  Serial.print("Time Check: " );

  Serial.print(data.AnalogCheck);

  Serial.print( "  Button 1: " );

  Serial.print(data.button1);

  Serial.print( "  Button 2: " );

  Serial.print(data.button2);

  Serial.print( "  Button 3: " );

  Serial.print(data.button3);

  Serial.print( "  Button 4: " );

  Serial.println(data.button4);

  Serial.print( "  Joystick X: " );

  Serial.print(data.joystickX);

  Serial.print( "  Joystick Y: " );

  Serial.print(data.joystickY);

  Serial.print( "  Joystick Button: " );

  Serial.println(data.joystickButton);

  Serial.println("");

}
