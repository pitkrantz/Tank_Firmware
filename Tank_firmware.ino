#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <Arduino.h>
#include <analogWrite.h>

#include <Adafruit_NeoPixel.h>

#define N_LEDS 16
#define PIN 10

Adafruit_NeoPixel ringlight = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

//This is a test to see how github branches work for arduino

//KnightRider Branch



#define PERIPHERAL_NAME                "(TEST) Tank OMAKE (TEST)"
#define SERVICE_UUID                "157E0550-1355-4D56-A677-2CE1E55E05B1"
#define CHARACTERISTIC_INPUT_UUID   "E0E23CC5-6675-4901-AB96-94AC4FE31361"
#define CHARACTERISTIC_OUTPUT_UUID  "44BB23E7-F4D5-4734-A885-7D82A0E851D6"

TaskHandle_t Task2;

int left = 16;
int leftpos = 18;
int leftneg = 19;

int right = 17;
int rightpos = 2;
int rightneg = 4;

int buzzerpin = 15;

int statusLightPin = 23;

int lightPin = 21;

bool idleMode = true;

//uint8_t Values[4] = {0x7F, 0x7F, 0x00, 0x00};

static uint8_t outputData[1];

BLECharacteristic *pOutputChar;

void ledblink(){
  for(byte i = 0; i < 0xFF; i++){
    analogWrite(statusLightPin, i);
    delay(5);
  }
  analogWrite(statusLightPin, 0);
  delay(100);
  
}

void idlePulsing(bool idleMode){
   if (!idleMode){
    delay(700);
    digitalWrite(statusLightPin, LOW);
  
  }
  else{
    ledblink();
  }
}

void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    if(!idleMode) {
      analogWrite(statusLightPin, 0);
    }
    else{
      for(byte i = 0; i < 0xFF; i++){
        analogWrite(statusLightPin, i);
        delay(5);
      }
    }
  }
}



/*
    if (!idleMode){
    delay(700);
    digitalWrite(statusLightPin, LOW);
  
  }
  else{
    //digitalWrite(statusLightPin, HIGH);
    //ledblink();
    while (true){
      for(int i = 0; i < 255; i + 10){
      analogWrite(statusLightPin, 128);
      delay(100);
      }
      analogWrite(statusLightPin, 0);
      delay(100);
    }
  }
 */

class ServerCallbacks: public BLEServerCallbacks{
  void onConnect(BLEServer* pServer) {
      Serial.println("BLE Client Connected");
      //idlePulsing(false);
      idleMode = false;
  }

  void onDisconnect(BLEServer* pServer) {
    BLEDevice::startAdvertising();
    Serial.println("BLE Client Disconnected");
    //idlePulsing(true);
    idleMode = true;
    digitalWrite(lightPin, LOW);
    
  }
};

class InputReceivedCallbacks: public BLECharacteristicCallbacks {
    void leftMotor(uint8_t input){
      if (input >= 0x7F){
        digitalWrite(leftpos, HIGH);
        digitalWrite(leftneg, LOW);

        int newvalue = (input - 127) *2;
        analogWrite(left, newvalue); 
      }
      if(input < 0x7F){
        digitalWrite(leftpos, LOW);
        digitalWrite(leftneg, HIGH);

        int newvalue = (input -127) * (-2);

        analogWrite(left, newvalue);
      }
    }

    void rightMotor(uint8_t input){
      if (input >= 0x7F){
        digitalWrite(rightpos, HIGH);
        digitalWrite(rightneg, LOW);

        int newvalue = (input - 127) *2;
        analogWrite(right, newvalue); 
      }
      else{
        digitalWrite(rightpos, LOW);
        digitalWrite(rightneg, HIGH);

        int newvalue = (input -127) * (-2);

        analogWrite(right, newvalue);
      }
    }

    void horn(uint8_t input){
      if(input > 0x00){
        analogWrite(buzzerpin, 150);
      }
      else{
        analogWrite(buzzerpin, LOW);
      }
    }

    void light(uint8_t input){
      if(input > 0x00) {
        analogWrite(lightPin, HIGH);
      }
      else {
        analogWrite(lightPin, LOW);
      }
    }



        //int numberOfLights = int(newValueL * 0,03);
        //for(int i = 0; i < numberOfLights; i++){
          //pixel.color(i, green)
        //}

    //optimisation einfach en domain vun 0-255/8 etc fir net emmer mussen ze rechnen
    // 0-31, 32-63, 64 - 95, 96-127, 128-159, 160-191, 192-223, 224-255


    int getLedNumber(int value){

      if (0<value<=31){
        Serial.println("1 LED");
        return 1;
      }

      else if (32<=value<=63){
        Serial.println("2 LED");
        return 1;
      }
      
      else if (64<=value<=95){
        Serial.println("3 LED");
        return 1;
      }
      
      else if (96<=value<=127){
        Serial.println("4 LED");
        return 1;
      }
      
      else if (128<=value<=159){
        Serial.println("5 LED");
        return 1;
      }
      
      else if (160<=value<=191){
        Serial.println("6 LED");
        return 1;
      }
      
      else if (192<=value<=223){
        Serial.println("7 LED");
        return 1;
      }
      
      else if (224<=value<=255){
        Serial.println("8 LED");
        return 1;
      }

      else{
        Serial.println("0 LED");
        return 0;
      }
      
    }

    

    void RingLight(uint8_t inputL, uint8_t inputR){
  
      if (inputL >= 0x7F){
        int newValueL = (inputL - 127) *2;
        int numberOfLeds = getLedNumber(newValueL);
        if(numberOfLeds <= 0){
          for (int i = 8; i < 17; i++) {
            ringlight.setPixelColor(i, 0,0,0);
          }
        }
        else{
          for(int i = 1; i < numberOfLeds; i++){
            ringlight.setPixelColor(17-i, 0, 255, 0);
            
          }
        }
        
        
      }
      else if (inputL < 0x7F){
        int newValueL = (inputL -127) * (-2);
        int numberOfLeds = getLedNumber(newValueL);
        if(numberOfLeds <= 0){
          for(int i = 8; i < 17; i++) {
            ringlight.setPixelColor(i, 0, 0, 0);
          }
        }
        else{
          for(int i = 8; i < numberOfLeds; i ++){
            ringlight.setPixelColor(i, 255, 0, 0);
          }
        }
        
      }

      if (inputR >= 0x7F){
        int newValueR = (inputR - 127) *2;
        int numberOfLeds = getLedNumber(newValueR);
        if (numberOfLeds <= 0){
          for(int i = 0; i < 8; i++) {
            ringlight.setPixelColor(i, 0, 0, 0);
          }
        }
        else{
          for (int i = 0; i < numberOfLeds; i++){
            ringlight.setPixelColor(i, 0, 255, 0);
          }
        }
        
      }
      else if (inputR < 0x7F){
        int newValueR = (inputR - 127) * (-2);
        int numberOfLeds = getLedNumber(newValueR);
        if (numberOfLeds <= 0) {
          for(int i = 0; i < numberOfLeds; i++) {
            ringlight.setPixelColor(i, 0, 0, 0);
          }
        }
        else{
          for(int i = 0; i < numberOfLeds; i++){
            ringlight.setPixelColor(8-i, 255, 0, 0);
          }
        }
       
      }
      
    }


    
    
    void onWrite(BLECharacteristic *pCharWriteState) {
      Serial.println("Receiving Data");
        uint8_t *inputValues = pCharWriteState->getData();
        

//        analogWrite(left, inputValues[0]);
        
        leftMotor(inputValues[0]);
        rightMotor(inputValues[1]);
        
        horn(inputValues[2]);

        light(inputValues[3]);

        RingLight(inputValues[0], inputValues[1]);
        
        
        //analogWrite(right, inputValues[1]);

        Serial.println(inputValues[0], DEC);
        Serial.println(inputValues[1], DEC);
        Serial.println(inputValues[2], DEC);
        Serial.println(inputValues[3], DEC);


   // Serial.printf("Sending response:  %02x\r\n", outputData[0]);

   // pOutputChar->setValue((uint8_t *)outputData, 1);
   // pOutputChar->notify();
  }
};


void intro(){
  
}

void setup(){
  idlePulsing(true);
  ringlight.begin();
  Serial.begin(115200);
  Serial.println("Begin Setup BLE Service and Characteristics");

  xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 


  pinMode(leftpos, OUTPUT);
  pinMode(leftneg, OUTPUT);

  pinMode(rightpos, OUTPUT);
  pinMode(rightneg, OUTPUT);

  pinMode(buzzerpin, OUTPUT);

  pinMode(statusLightPin, OUTPUT);

  pinMode(lightPin, OUTPUT);

  // Configure thes server

  BLEDevice::init(PERIPHERAL_NAME);
  BLEServer *pServer = BLEDevice::createServer();

  // Create the service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a characteristic for the service
  BLECharacteristic *pInputChar = pService->createCharacteristic(
                              CHARACTERISTIC_INPUT_UUID,                                        
                              BLECharacteristic::PROPERTY_WRITE_NR | BLECharacteristic::PROPERTY_WRITE);

  pOutputChar = pService->createCharacteristic(
                              CHARACTERISTIC_OUTPUT_UUID,
                              BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  pServer->setCallbacks(new ServerCallbacks());
  pInputChar->setCallbacks(new InputReceivedCallbacks());

  outputData[0] = 0x00;
  pOutputChar->setValue((uint8_t *)outputData, 1);

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); 
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("Ble Service is advertising");

  
}




void loop(){
  delay(2000);
  
}
