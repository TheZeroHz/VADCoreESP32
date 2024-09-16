#ifndef VADCOREESP32_H
#define VADCOREESP32_H

#include <Arduino.h>
#include "arduinoFFT.h"
#include "driver/i2s.h"

#define VAD_VOICE true
#define VAD_SILENCE false
#define VAD_SAMPLE_RATE 16000
#define FFT_SIZE 1024
#define SPEECH_THRESHOLD 3000
#define NOISE_THRESHOLD 1000  //within 800-1200
#define SPEECH_FREQ_MIN 300
#define SPEECH_FREQ_MAX 3400
#define GAIN_FACTOR 1.5  // Gain factor (adjust as needed)

// Queue size to pass I2S data between tasks
#define I2S_QUEUE_SIZE 10

class VADCoreESP32 {
public:
    VADCoreESP32() : coreId(xPortGetCoreID()), priority(1), vadTaskHandle(NULL), i2sQueue(NULL) {}
    void start();
    void stop();
    bool isRunning();
    void setMaxTime(unsigned long maxTime);
    void setBonusTime(unsigned long bonusTime);
    void setCore(int coreId);  // Set core for task
    void setPriority(UBaseType_t priority); // Set priority for task
    void passAudioToVAD(int16_t *buffer, size_t size); // Add function to pass audio to the VAD task
    
private:
    int coreId;
    UBaseType_t priority;
    float previousEnergy = 0;
    double vReal[FFT_SIZE];
    double vImag[FFT_SIZE];
    ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, FFT_SIZE, VAD_SAMPLE_RATE);
    QueueHandle_t i2sQueue;  // Queue for passing I2S data
    TaskHandle_t vadTaskHandle;
    unsigned long maxTime=10000;  // Maximum recording time in milliseconds
    unsigned long bonusTime=3000; // Bonus time in milliseconds
    unsigned long startTime = 0;
    bool recording = false;
    bool bonusStarted = false;
    bool listening = false;
    void apply_gain(int16_t *data, size_t length);
    float calculateEnergy(const double *data, int len);
    bool isSpeechDetected();
    float smoothValue(float newValue, float oldValue, float alpha);
    bool vadDetect();
    static void vadTaskWrapper(void *pvParameters); // Wrapper for FreeRTOS task
    void vadTask();
};

#endif
