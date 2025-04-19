
#ifndef _mems
#define _mems
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "Ticker.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define SAMPLE_RATE 512
// #define SAMPLE_RATE 600
#define NUM_SAMPLES 256   // Number of samples to collect
#define FIR_ORDER 33     // Order of the FIR filter
#define NUM_PEAKS 10
#define THRESH_ACT_LBS 62.5     //62.5mg/lsb
#define ADXL_INT1 12  // Moved from GPIO26 → GPIO12
#define ADXL_INT2 14  // Moved from GPIO27 → GPIO14

typedef struct{
    float gReal[NUM_SAMPLES];
    float gImag[NUM_SAMPLES];
    float Filter[NUM_SAMPLES];
    float Velocity[NUM_SAMPLES];  
    float gFFT[NUM_SAMPLES];
    float gRaw[NUM_SAMPLES];
  
    double sumSqrVelocity;
    double RmsVelocity;
    double sumsqrG;
    double RmsG;
    int32_t Calib;
    int8_t OFST;
    float ADJ_OFST;
    float peakFreq[NUM_PEAKS];  // Array to store top 10 peak frequencies
    float peakValue[NUM_PEAKS]; // Array to store top 10 peak values
    int ipeak_Freq[NUM_PEAKS];   // Array to store peak indices
  
    float Gain_velocity;
  }type_axis_data;

typedef struct {
    type_axis_data x;
    type_axis_data y;
    type_axis_data z;
    double roll;    // euler anble name of  x 
    double pitch;   // euler anble name of  y
    double yaw;     // euler anble name of  z
    double Inclination;   // the angle between the gravity vector and a particular axis (often the Z-axis is used as the reference).
    int index;
    unsigned long tick_task;
    unsigned long sample_tick;
    unsigned long sample_t;
    int CAL_RDY;
    void (*read_mems_task)(void);
    void (*initial_mems_task)(void);
    void (*calibrate_chip_OSFT)(void);
    void (*calibrate_pg_OSFT)(void);
  }virbration_mems;


class mems{
    public:
    mems(unsigned long tkinterval);
    void set_mems_data(virbration_mems *psource);
    uint16_t raw_adc;
    float Vbat;
    

    //==========  Acc mems ===========//
    Adafruit_ADXL345_Unified accel;
    sensor_t sensor;
    sensors_event_t event;
    virbration_mems mems_data;
    virbration_mems *pmems_data;
    const uint16_t mems_samples = NUM_SAMPLES; //This value MUST ALWAYS be a power of 2
    // const double samplingFrequency = SAMPLE_RATE;
    // const uint8_t amplitude = 100;
    // const uint16_t samples = 97; //This value MUST ALWAYS be a power of 2
    // mems config //
    int16_t mems_sampling_rage;
    int16_t mems_g_range;
    float mems_task_period;
    int16_t mems_cmd;
    int16_t mems_THRESH_ACT;    //62.5 mg
    uint8_t mems_INT_SOURCE;
    volatile bool motionDetected = false;
    volatile bool motion_weak_up =false;
    
    void displaySensorDetails(void);
    void displayDataRate(void);
    void displayRange(void);
    void init_adxl3245_module();
    void Adxl345_calibrate();
    void Adxl345_setINT();
    void calibrate_adj();
    double calculate_rms_value(double *raw_data,int index);
    void calculate_RMS_velocity();
    void calculate_RMS_g();
    float Calculate_Inclination(virbration_mems *mems);
    void calculate_rotation(virbration_mems *mems);
    void Task_read_mems();
    void Print_FFT();
    void PrintRawData();
    //==========  Acc mems ===========//

    private:
    unsigned long   _lastMillis;      // Tracks time for periodic tasks
    unsigned long   _interval;        // Interval for background task
    Ticker taskmems;
    static void periodicTaskWrapper(mems* instance);
};

extern mems vn_mems;
extern virbration_mems mems_data;
#endif
