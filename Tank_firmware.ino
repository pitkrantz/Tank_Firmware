#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <Arduino.h>
#include <analogWrite.h>

#include <Adafruit_NeoPixel.h>

#define LEDPIN  27
#define N_LEDS  16

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(N_LEDS, LEDPIN, NEO_GRB + NEO_KHZ800);

//This is a test to see how github branches work for arduino
// Yes KnightRider



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
      if(0<= value && value < 16){
        Serial.println("8");
        return 8;
      }
      
      if(16<= value && value < 32){
        Serial.println("7");
        return 7;
      }
      
      if(32<= value && value < 48){
        Serial.println("6");
        return 6;
      }
      
      if(48<= value && value < 64){
        Serial.println("5");
        return 5;
      }
      
      if(64<= value && value < 80){
        Serial.println("4");
        return 4;
      }
      
      if(80<= value && value < 96){
        Serial.println("3");
        return 3;
      }

      if(96<= value && value < 112){
        Serial.println("2");
        return 2;
      }

      if(112<= value && value < 127){
        Serial.println("1");
        return 1;
      }

      if(value == 127){
        Serial.println("zerro");
        return 0;
      }

      if(127< value && value < 144){
        Serial.println("1");
        return 1;
      }

      if(144<= value && value < 160){
        Serial.println("2");
        return 2;
      }

      if(160<= value && value < 176){
        Serial.println("3");
        return 3;
      }

      if(176<= value && value < 192){
        Serial.println("4");
        return 4;
      }

      if(192<= value && value < 208){
        Serial.println("5");
        return 5;
      }

      if(208<= value && value < 224){
        Serial.println("6");
        return 6;
      }

      if(224<= value && value < 240){
        Serial.println("7");
        return 7;
      }

      if(240<= value && value <= 255){
        Serial.println("8");
        return 8;
      }
    }

    void LeftLight(uint8_t inputL){
      int numLeds = getLedNumber(inputL);
      if (numLeds == 0){
        for(int i = 0; i < 8; i++){
          pixels.setPixelColor(i, 0, 0, 0);
        }
      }
      if (inputL > 127){
        for(int i = 0; i < numLeds; i++){
          pixels.setPixelColor(i, 0, 255, 0);
        }
        for(int i = numLeds + 1; i < 8; i++){
          pixels.setPixelColor(i, 0, 0, 0);
        }
        
      }
      if (inputL < 127) {
        for(int i = 8 - numLeds; i < 8; i++){
          pixels.setPixelColor(i, 255, 0, 0);
        }
        for(int i = 0; i < 8 - numLeds; i++){
          pixels.setPixelColor(i, 0, 0, 0);
        }
      }
    
      pixels.show();
    }

    void RightLight(uint8_t inputR){
      int numLeds = getLedNumber(inputR);
      if (numLeds == 0){
        for(int i = 8; i < 16; i++){
          pixels.setPixelColor(i, 0, 0, 0);
        }
      }
      if (inputR > 127){
        for(int i = 16 - numLeds; i < 16; i++){
          pixels.setPixelColor(i, 0, 255, 0);
        }
        for(int i = 8; i < 16 - numLeds; i++){
          pixels.setPixelColor(i, 0, 0, 0);
        }
      }
      if (inputR < 127) {
        for(int i = 8; i < 8 + numLeds; i++){
          pixels.setPixelColor(i, 255, 0, 0);
        }
        for(int i = 8 + numLeds; i < 16; i++){
          pixels.setPixelColor(i, 0, 0, 0);
        }
      }
    
      pixels.show();
    }  

    
    void onWrite(BLECharacteristic *pCharWriteState) {
      Serial.println("Receiving Data");
        uint8_t *inputValues = pCharWriteState->getData();
        

//        analogWrite(left, inputValues[0]);
        
        leftMotor(inputValues[0]);
        rightMotor(inputValues[1]);
        
        horn(inputValues[2]);

        //light(inputValues[3]);

        LeftLight(inputValues[0]);
        RightLight(inputValues[1]);

        
        
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

  pixels.begin();

  
  idlePulsing(true);
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
