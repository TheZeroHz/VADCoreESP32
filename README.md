

---

# VADCoreESP32

**Version:** 1.0.2  
**Author:** Rakib Hasan 
**Category:** Audio Data Processing  

## Overview

`VADCoreESP32` is a library for Voice Activity Detection (VAD) on ESP32 devices. It utilizes floating-point Fast Fourier Transform (FFT) calculations to detect speech activity in real-time. This library is optimized for the ESP32 hardware, providing efficient audio processing and VAD functionality.

## Features

- Real-time Voice Activity Detection using FFT
- Configurable core and priority for FreeRTOS tasks
- Gain adjustment and noise thresholding
- Flexible I2S configuration for audio input
- Smooth energy calculation for improved detection accuracy

## Installation

1. **Download the Library**

   You can download the latest version of `VADCoreESP32` from the [GitHub repository](https://github.com/TheZeroHz/VADCoreESP32).

2. **Add to Your Arduino Libraries**

   - Copy the `VADCoreESP32` folder into your Arduino `libraries` directory.
   - Restart the Arduino IDE if it was open during the copy.

## Usage

### Example Sketch

```cpp
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
    myVad.setCore(0); // Set to Core 0 (Optional, if your board has 2 cores)
    myVad.setPriority(10); // Set task priority (Optional, if your board supports different priorities)
}

void loop() {
    if (Serial.available() > 0) {
        String cmd = Serial.readString();
        if (cmd.compareTo("start") == 0) {
            myVad.start(); // Start VAD task
            Serial.println("Start Listening Command Received");
        }
    }

    if (myVad.getState() == VAD_SILENCE) {
        Serial.println("[Idle Heap:" + String(ESP.getFreeHeap()) + " Core:" + String(xPortGetCoreID()) + "]");
    }

    delay(100);
}
```

### Class Methods

- **`void setCore(int coreId)`**

  Sets the core for the VAD task. Accepts `0` or `1` for ESP32 cores.

- **`void setPriority(UBaseType_t priority)`**

  Sets the priority of the VAD task. Use a value between `0` (lowest) and `24` (highest).

- **`bool getState()`**

  Returns the current state of the VAD. Returns `VAD_VOICE` if speech is detected, otherwise `VAD_SILENCE`.
  When `VAD_SILENCE` triggers  VAD Task Gets Automatically got Deleted. 

- **`void start()`**

  Initializes and starts the VAD task with the configured core and priority.


### Configuration

- **`I2S_SAMPLE_RATE`**: The sample rate for I2S audio input. Default is `16000`.
- **`FFT_SIZE`**: The size of the FFT. Default is `256`.
- **`SPEECH_THRESHOLD`**: The threshold for detecting speech activity. Default is `3000`.
- **`NOISE_THRESHOLD`**: The threshold for distinguishing noise. Default is `1000`.
- **`SPEECH_FREQ_MIN`** and **`SPEECH_FREQ_MAX`**: The frequency range for detecting speech. Defaults are `300` Hz and `3400` Hz, respectively.
- **`GAIN_FACTOR`**: The gain factor for audio samples. Default is `1.5`.

## License

This library is released under the MIT License. See the [LICENSE](LICENSE) file for details.

---
