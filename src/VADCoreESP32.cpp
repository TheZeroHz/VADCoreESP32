#include "VADCoreESP32.h"

void VADCoreESP32::setCore(int coreId) {
    this->coreId = coreId;
}

void VADCoreESP32::setPriority(UBaseType_t priority) {
    this->priority = priority;
}

bool VADCoreESP32::isRunning() {
    return recording;
}


void VADCoreESP32::setMaxTime(unsigned long maxTime) {
    this->maxTime = maxTime;
}

void VADCoreESP32::setBonusTime(unsigned long bonusTime) {
    this->bonusTime = bonusTime;
}

void VADCoreESP32::apply_gain(int16_t *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        data[i] = (int16_t)(data[i] * GAIN_FACTOR);
        if (data[i] > INT16_MAX) data[i] = INT16_MAX;
        if (data[i] < INT16_MIN) data[i] = INT16_MIN;
    }
}

void VADCoreESP32::passAudioToVAD(int16_t *buffer, size_t size) {
    if (i2sQueue != NULL) {
        xQueueSend(i2sQueue, buffer, portMAX_DELAY);
    }
}

bool VADCoreESP32::vadDetect() {
    int16_t i2sBuffer[FFT_SIZE];
    
    if (xQueueReceive(i2sQueue, &i2sBuffer, portMAX_DELAY) == pdTRUE) {
        apply_gain(i2sBuffer, FFT_SIZE);

        for (int i = 0; i < FFT_SIZE; i++) {
            vReal[i] = (double)i2sBuffer[i];
            vImag[i] = 0;
        }

        FFT.windowing(vReal, FFT_SIZE, FFTWindow::Hamming, FFTDirection::Forward, vReal, false);
        FFT.compute(vReal, vImag, FFT_SIZE, FFTDirection::Forward);
        FFT.complexToMagnitude(vReal, vImag, FFT_SIZE);
        memset(i2sBuffer, 0, sizeof(i2sBuffer));
        return isSpeechDetected();
    }
    return false;
}

bool VADCoreESP32::isSpeechDetected() {
    float energy = 0;
    int startIndex = (SPEECH_FREQ_MIN * FFT_SIZE) / VAD_SAMPLE_RATE;
    int endIndex = (SPEECH_FREQ_MAX * FFT_SIZE) / VAD_SAMPLE_RATE;

    for (int i = startIndex; i < endIndex; i++) {
        energy += vReal[i] * vReal[i];
    }

    return (sqrt(energy / (endIndex - startIndex)) > SPEECH_THRESHOLD);
}

void VADCoreESP32::vadTask() {
    unsigned long startTime = millis();
    unsigned long lastDetectionTime = startTime;
    unsigned long elapsedTime = 0;

    while (listening) {
        unsigned long currentTime = millis();

        if (vadDetect()) {
            lastDetectionTime = currentTime; // Reset last detection time when speech is detected
            Serial.println("Speech Detected on Core " + String(xPortGetCoreID()));
        }

        // Calculate total elapsed time since the recording started
        elapsedTime = currentTime - startTime;

        if (elapsedTime >= maxTime) {
            Serial.println("Max listening time reached.");
            recording = false;
            listening = false;
            break; // Stop listening once max time is reached
        }

        // Check if current time has exceeded the allowed bonus time after the last speech detection
        if (currentTime - lastDetectionTime >= bonusTime) {
            Serial.println("Bonus time expired.");
            recording = false;
            listening = false;
            break; // Stop listening if bonus time has expired
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);  // Avoid task starvation
    }
}

void VADCoreESP32::vadTaskWrapper(void *pvParameters) {
    VADCoreESP32 *instance = (VADCoreESP32 *)pvParameters;
    instance->vadTask();
    Serial.println("Deleting VAD....");
    vTaskDelete(instance->vadTaskHandle);
}

void VADCoreESP32::start() {
   // Create a queue for passing I2S data between cores
    i2sQueue = xQueueCreate(I2S_QUEUE_SIZE, sizeof(int16_t) * FFT_SIZE);
    //vTaskDelay(10 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(
        vadTaskWrapper,
        "VADCoreESP32 Task",
        4096,
        this,
        priority,
        &vadTaskHandle,
        coreId
    );

    listening = true;
    recording = true;
}

void VADCoreESP32::stop(){
  vTaskDelete(vadTaskHandle);
}
