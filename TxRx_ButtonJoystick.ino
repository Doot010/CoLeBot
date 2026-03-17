#include "WifiPort2.h"



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
  //int OtherThing;
  //float AndAnotherThing;
  //char YetAnotherThing;


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
    pinMode(3,INPUT_PULLUP);//Button1
    pinMode(4,INPUT_PULLUP);//Button2
    pinMode(5,INPUT_PULLUP);//Button3
    pinMode(6,INPUT_PULLUP);//Button4
    pinMode(A0, INPUT);//JoystickX
    pinMode(A1, INPUT);//JoystickY
    pinMode(2,INPUT_PULLUP);//JoystickButton
  }
}

void loop() {

  if (WifiSerial.getPortType() == WifiPortType::Transmitter || WifiSerial.getPortType() == WifiPortType::Emulator) {
    WifiSerial.autoReconnect();//try and connect

    //Tx stuff below
    data.AnalogCheck = millis(); // analogRead(A0);
    Serial.print("Check Sending: " );
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
    Serial.print("Check Recieving: " );
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
    //all RX stuff above

  }

  delay(100); // update delay after you get it working to be a smaller number like 10ms to account for WiFi transmission overhead
}
