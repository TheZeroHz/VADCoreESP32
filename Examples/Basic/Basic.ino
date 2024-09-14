#include <VADCoreESP32.h>
// Define your I2S pin configuration
#define I2S_PORT I2S_NUM_0
#define I2S_WS 16
#define I2S_SD 7
#define I2S_SCK 15

VADCoreESP32 myVad;
void setup() {
    Serial.begin(115200);
    myVad.i2sInit(I2S_PORT, I2S_SCK, I2S_WS, I2S_SD);
    myVad.setCore(0); //Comment This If your board Don't have 2 cores
    myVad.setPriority(10); //Comment This If your board Don't have 2 cores
}

void loop() {
     if (Serial.available() > 0) {
            String cmd = Serial.readString();
            if (cmd.compareTo("start") == 0) {
                myVad.start(); // Start VAD task
                Serial.println("Start Listening Command Received");
            }
        }
        if(myVad.getState()==VAD_SILENCE){
        Serial.println("[Idle Heap:"+String(ESP.getFreeHeap())+" Core:"+String(xPortGetCoreID())+"]");  
        }
        delay(100);
}
