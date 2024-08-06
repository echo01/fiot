#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include "arduinoFFT.h"

#include "sensor_board.h"

#define BATTERY_PIN 36 // GPIO36 is ADC1_CH0 on ESP32
#define ADC_MAX 4095   // Maximum ADC value for 12-bit resolution
#define VREF 3.67       // Reference voltage of ADC, usually 3.3V for ESP32

#define Rvbat1  91000
#define Rvbat2  33000
#define Divider_ration (Rvbat1+Rvbat2)/Rvbat2


/*
These values can be changed in order to evaluate the functions
*/
const uint16_t samples = 97; //This value MUST ALWAYS be a power of 2
const double signalFrequency = 1000;
const double samplingFrequency = 800;
const uint8_t amplitude = 100;

/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
#define SAMPLE_RATE 800
#define NUM_SAMPLES 64   // Number of samples to collect
#define FIR_ORDER 33     // Order of the FIR filter
// Arrays to store data
double velocity[samples];
double fftSpectrum[samples/ 2]; // Array to store the FFT spectrum

double xReal[samples];
double xImag[samples];

double yReal[samples];
double yImag[samples];

double zReal[samples];
double zImag[samples];

double xFiltered[samples], yFiltered[samples], zFiltered[samples];
// FIR filter coefficients for a low-pass filter with cutoff frequency 400 Hz
const double firCoefficients[FIR_ORDER] = {
  0.0064, 0.0080, 0.0098, 0.0118, 0.0140, 0.0163, 0.0187, 0.0212, 0.0238, 0.0264,
  0.0291, 0.0317, 0.0342, 0.0367, 0.0390, 0.0411, 0.0430, 0.0447, 0.0461, 0.0472,
  0.0480, 0.0484, 0.0484, 0.0480, 0.0472, 0.0461, 0.0447, 0.0430, 0.0411, 0.0390,
  0.0367, 0.0342, 0.0317
};

ArduinoFFT<double> FFT = ArduinoFFT<double>(xReal, xImag, samples, samplingFrequency);
#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
sensor_t sensor;
sensors_event_t event;


type_st01 st01={
    .humidity=0,
    // .temperature=0,
    // .tick_ms_airsensor=0,
    // .voc_index=0,
    // .voc_raw=0,
    .initial_adcBattery=&initial_adc_vbat,
    .read_task=&Task_st01_read,
    .read_mems_task=&Task_read_mems
    
};

type_mqtt_value mqtt_node;

void initial_adc_vbat()
{
  analogReadResolution(12); // Set ADC resolution to 12 bits
  analogSetAttenuation(ADC_11db); // Set attenuation to 11dB for higher voltage range
}

float calculate_vbat()
{
  // R divider ratio is 91K/33k
  //  ir33 = Vin/((91+33)*1000)
  //  Vadc =(33*1000)*{Vin/((91+33)*1000)}
  //  Vin = Vbat
  //  Vbat = (91+33)/33 * Vadc
  float vadc = st01.raw_adc * (VREF / ADC_MAX); // Convert ADC value to voltage
  return(vadc*Divider_ration);
}

void Task_st01_read()
{
  // st01.temperature = sht31.readTemperature();
  // st01.humidity = sht31.readHumidity();
  // st01.voc_index = sgp.measureVocIndex(st01.temperature, st01.humidity);
  st01.raw_adc = analogRead(BATTERY_PIN); // Read the ADC value
  st01.Vbat= calculate_vbat();
  

  if (millis() - st01.tick_ms_airsensor >= 5000) {
    st01.tick_ms_airsensor = millis();
    Serial.print("Vbattery = \t");
    Serial.println(st01.Vbat);
    Serial.print("ADC = \t");
    Serial.println(st01.raw_adc);
    }
}

void displaySensorDetails(void)
{
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void displayDataRate(void)
{
  Serial.print  ("Data Rate:    "); 
  
  switch(accel.getDataRate())
  {
    case ADXL345_DATARATE_3200_HZ:
      Serial.print  ("3200 "); 
      break;
    case ADXL345_DATARATE_1600_HZ:
      Serial.print  ("1600 "); 
      break;
    case ADXL345_DATARATE_800_HZ:
      Serial.print  ("800 "); 
      break;
    case ADXL345_DATARATE_400_HZ:
      Serial.print  ("400 "); 
      break;
    case ADXL345_DATARATE_200_HZ:
      Serial.print  ("200 "); 
      break;
    case ADXL345_DATARATE_100_HZ:
      Serial.print  ("100 "); 
      break;
    case ADXL345_DATARATE_50_HZ:
      Serial.print  ("50 "); 
      break;
    case ADXL345_DATARATE_25_HZ:
      Serial.print  ("25 "); 
      break;
    case ADXL345_DATARATE_12_5_HZ:
      Serial.print  ("12.5 "); 
      break;
    case ADXL345_DATARATE_6_25HZ:
      Serial.print  ("6.25 "); 
      break;
    case ADXL345_DATARATE_3_13_HZ:
      Serial.print  ("3.13 "); 
      break;
    case ADXL345_DATARATE_1_56_HZ:
      Serial.print  ("1.56 "); 
      break;
    case ADXL345_DATARATE_0_78_HZ:
      Serial.print  ("0.78 "); 
      break;
    case ADXL345_DATARATE_0_39_HZ:
      Serial.print  ("0.39 "); 
      break;
    case ADXL345_DATARATE_0_20_HZ:
      Serial.print  ("0.20 "); 
      break;
    case ADXL345_DATARATE_0_10_HZ:
      Serial.print  ("0.10 "); 
      break;
    default:
      Serial.print  ("???? "); 
      break;
  }  
  Serial.println(" Hz");  
}

void displayRange(void)
{
  Serial.print  ("Range:         +/- "); 
  
  switch(accel.getRange())
  {
    case ADXL345_RANGE_16_G:
      Serial.print  ("16 "); 
      break;
    case ADXL345_RANGE_8_G:
      Serial.print  ("8 "); 
      break;
    case ADXL345_RANGE_4_G:
      Serial.print  ("4 "); 
      break;
    case ADXL345_RANGE_2_G:
      Serial.print  ("2 "); 
      break;
    default:
      Serial.print  ("?? "); 
      break;
  }  
  Serial.println(" g");  
}

void init_adxl3245_module()
{

  /* Initialise the sensor */
#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif
  // Serial.begin(9600);
  Serial.println("Accelerometer Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  // accel.setRange(ADXL345_RANGE_16_G);
  accel.setRange(ADXL345_RANGE_8_G);
  // accel.setRange(ADXL345_RANGE_4_G);
  // accel.setRange(ADXL345_RANGE_2_G);
  accel.setDataRate(ADXL345_DATARATE_800_HZ);
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  /* Display additional settings (outside the scope of sensor_t) */
  displayDataRate();
  displayRange();
  Serial.println("");
}

void Task_read_mems()
{

}


