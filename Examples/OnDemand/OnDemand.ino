/////// THIS IS FAR ACCURATE /////

#include <VADCoreESP32.h>
// Define your I2S pin configuration
#define I2S_PORT I2S_NUM_0
#define I2S_WS 16
#define I2S_SD 7
#define I2S_SCK 15

VADCoreESP32 myVad;

void i2sInit(int i2sPort, int i2sBckPin, int i2sWsPin, int i2sDataPin);
void setup() {
    Serial.begin(115200);
    i2sInit(I2S_PORT, I2S_SCK, I2S_WS, I2S_SD);
    myVad.setCore(0);  //default is set to xPortGetCoreID()
    myVad.setPriority(10); //default 1,Higher the value more prior the task is
    myVad.setMaxTime(12000); // default is 10000ms or 10s
    myVad.setBonusTime(3000);// defaul is 3000ms or 3s
    Serial.println("SetUp on Core " + String(xPortGetCoreID()));
}
String CMD="";
void loop() {
    size_t bytesRead;
    int16_t AudioBuffer[FFT_SIZE];

    if (Serial.available() > 0)CMD=Serial.readString();
    if(CMD.compareTo("start")==0){myVad.start();CMD="";Serial.println("START RECORDING");}
    i2s_read(I2S_PORT, (char *)AudioBuffer, FFT_SIZE * sizeof(int16_t), &bytesRead, portMAX_DELAY);// Read I2S data in the loop
    if(myVad.isRunning()){
      //RECORD(AudioBuffer)--> Your Desired Task
      myVad.passAudioToVAD(AudioBuffer, FFT_SIZE);  // Pass audio to VAD task
    }
    else Serial.println("STOP RECORDING");
}


void i2sInit(int i2sPort, int i2sBckPin, int i2sWsPin, int i2sDataPin) {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = VAD_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t (I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_driver_install(i2s_port_t(i2sPort), &i2s_config, 0, NULL);

    const i2s_pin_config_t pin_config = {
        .bck_io_num = i2sBckPin,
        .ws_io_num = i2sWsPin,
        .data_out_num = -1,
        .data_in_num = i2sDataPin
    };

    i2s_set_pin(i2s_port_t(i2sPort), &pin_config);
}

