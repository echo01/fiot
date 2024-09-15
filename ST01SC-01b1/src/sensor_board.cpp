#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include "arduinoFFT.h"

#include "sensor_board.h"
#include "ProessFFT.h"

#define BATTERY_PIN 36 // GPIO36 is ADC1_CH0 on ESP32
#define ADC_MAX 4095   // Maximum ADC value for 12-bit resolution
#define VREF 3.67       // Reference voltage of ADC, usually 3.3V for ESP32

#define Rvbat1  91000
#define Rvbat2  33000
#define Divider_ration (Rvbat1+Rvbat2)/Rvbat2


/*
These values can be changed in order to evaluate the functions
*/
// #define SAMPLE_RATE 800
// // #define SAMPLE_RATE 600
// #define NUM_SAMPLES 64   // Number of samples to collect
// #define FIR_ORDER 33     // Order of the FIR filter
// /*
// These values can be changed in order to evaluate the functions
// */
// const uint16_t mems_samples = NUM_SAMPLES; //This value MUST ALWAYS be a power of 2
// // const double signalFrequency = SAMPLE_RATE;
// const double samplingFrequency = SAMPLE_RATE;
// const uint8_t amplitude = 100;
// const uint16_t samples = 97; //This value MUST ALWAYS be a power of 2

/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
// #define SAMPLE_RATE 800
// #define NUM_SAMPLES 64   // Number of samples to collect
// #define FIR_ORDER 33     // Order of the FIR filter
// Arrays to store data
// double velocity[samples];
// double fftSpectrum[samples/ 2]; // Array to store the FFT spectrum

// double xReal[samples];
// double xImag[samples];

// double yReal[samples];
// double yImag[samples];

// double zReal[samples];
// double zImag[samples];

// double xFiltered[samples], yFiltered[samples], zFiltered[samples];
// FIR filter coefficients for a low-pass filter with cutoff frequency 400 Hz
// const double firCoefficients[FIR_ORDER] = {
//   0.0064, 0.0080, 0.0098, 0.0118, 0.0140, 0.0163, 0.0187, 0.0212, 0.0238, 0.0264,
//   0.0291, 0.0317, 0.0342, 0.0367, 0.0390, 0.0411, 0.0430, 0.0447, 0.0461, 0.0472,
//   0.0480, 0.0484, 0.0484, 0.0480, 0.0472, 0.0461, 0.0447, 0.0430, 0.0411, 0.0390,
//   0.0367, 0.0342, 0.0317
// };

// ArduinoFFT<double> FFT = ArduinoFFT<double>(xReal, xImag, samples, samplingFrequency);
// #define SCL_INDEX 0x00
// #define SCL_TIME 0x01
// #define SCL_FREQUENCY 0x02
// #define SCL_PLOT 0x03
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
sensor_t sensor;
sensors_event_t event;

extern virbration_mems mems_data;
extern void init_adxl3245_module();
extern void Adxl345_calibrate();
extern void calibrate_adj();
extern void Task_read_mems();

extern void displaySensorDetails(void);
extern void displayDataRate(void);
extern void displayRange(void);

extern void calculate_rotation(virbration_mems *mems);
extern void calculate_RMS_g();
extern void calculate_RMS_velocity();
extern double calculate_rms_value(double *raw_data,int index);
void Task_adxl345_mems();

virbration_mems mems_data={
    .read_mems_task=&Task_read_mems,
    .initial_mems_task=&init_adxl3245_module,
    .calibrate_chip_OSFT=&Adxl345_calibrate,
    .calibrate_pg_OSFT=&calibrate_adj,
};

type_st01 st01={
    .humidity=0,
    // .temperature=0,
    // .tick_ms_airsensor=0,
    // .voc_index=0,
    // .voc_raw=0,
    .initial_adcBattery=&initial_adc_vbat,
    .read_task=&Task_st01_read,
    .init_adxl345_module=&init_adxl3245_module,
    .read_adxl345_task=&Task_adxl345_mems
    
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
  mqtt_node.rssi=st01.net.rssi;
  mqtt_node.Vbat=st01.Vbat;

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
  st01.mems_g_range=8;
  st01.mems_sampling_rage=800;
  st01.mems_task_period=1000/st01.mems_sampling_rage;
  switch(st01.mems_g_range)
  {
    default:
    case 16:
      accel.setRange(ADXL345_RANGE_16_G);
      break;
    case 8:
      accel.setRange(ADXL345_RANGE_8_G);
      break;
    case 4:
      accel.setRange(ADXL345_RANGE_4_G);
      break;
    case 2:
      accel.setRange(ADXL345_RANGE_2_G);
      break;
  }
  // accel.setRange(ADXL345_RANGE_16_G);
  // accel.setRange(ADXL345_RANGE_8_G);
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


void Adxl345_calibrate()
{
  Serial.println("Start Calibrate Adx345 by place mems at z = 1 g, x=0 g, y=0 g\r\n");
  delayMicroseconds(1000000 / 10000);
  for (int i = 0; i < 10; i++) {
    accel.getEvent(&event);
    mems_data.x.Calib+=accel.getX();
    mems_data.y.Calib+=accel.getY();
    mems_data.z.Calib+=accel.getZ();
    delayMicroseconds(1000000 / 512);
  }
  mems_data.x.Calib=0;
  mems_data.y.Calib=0;
  mems_data.z.Calib=0;
  for (int i = 0; i < 100; i++) {
    delayMicroseconds(1000000 / 512);
    accel.getEvent(&event);
    mems_data.x.Calib+=accel.getX();
    mems_data.y.Calib+=accel.getY();
    mems_data.z.Calib+=accel.getZ();
    
  }
  //
  mems_data.x.Calib/=100;
  mems_data.y.Calib/=100;
  mems_data.z.Calib/=100;

  Serial.print("Cali_sum\t");
  Serial.print(mems_data.x.Calib);
  Serial.print("\t");
  Serial.print(mems_data.y.Calib);
  Serial.print("\t");
  Serial.print(mems_data.z.Calib);
  Serial.print("\r\n");
  //
  mems_data.x.OFST=-(mems_data.x.Calib/4);
  mems_data.y.OFST=-(mems_data.y.Calib/4);
  mems_data.z.OFST=-((mems_data.z.Calib-256)/4);
  // mems_data.z.OFST=-((mems_data.z.Calib)/4)-3;
  //
  accel.writeRegister(ADXL345_REG_OFSX, mems_data.x.OFST);
  accel.writeRegister(ADXL345_REG_OFSY, mems_data.y.OFST);
  accel.writeRegister(ADXL345_REG_OFSZ, mems_data.z.OFST);
  Serial.print("OFS\t");
  Serial.print(mems_data.x.OFST);
  Serial.print("\t");
  Serial.print(mems_data.y.OFST);
  Serial.print("\t");
  Serial.print(mems_data.z.OFST);
  Serial.print("\r\n");
}

void calibrate_adj()
{
  mems_data.x.ADJ_OFST=0;
  mems_data.y.ADJ_OFST=0;
  mems_data.z.ADJ_OFST=0;
  for (int i = 0; i < mems_samples; i++) {
    accel.getEvent(&event);

    // Save the data into arrays
    mems_data.x.gReal[i]=event.acceleration.x;
    // mems_data.x.gReal[i]=accel.getX();
    mems_data.x.gImag[i]= 0.0;
    mems_data.y.gReal[i]=event.acceleration.y;
    // mems_data.y.gReal[i]=accel.getY();
    mems_data.y.gImag[i]= 0.0;
    // mems_data.z.gReal[i]=event.acceleration.z;
    mems_data.z.gReal[i]=event.acceleration.z-SENSORS_GRAVITY_STANDARD;
    // mems_data.z.gReal[i]=accel.getZ();
    mems_data.z.gImag[i]= 0.0;

    mems_data.x.ADJ_OFST+=mems_data.x.gReal[i];
    mems_data.y.ADJ_OFST+=mems_data.y.gReal[i];
    mems_data.z.ADJ_OFST+=mems_data.z.gReal[i];
    delayMicroseconds(1000000 / SAMPLE_RATE);
  }
  
  mems_data.x.ADJ_OFST=mems_data.x.ADJ_OFST/mems_samples;
  mems_data.y.ADJ_OFST=mems_data.y.ADJ_OFST/mems_samples;
  mems_data.z.ADJ_OFST=mems_data.z.ADJ_OFST/mems_samples;
}

void Print_FFT()
{
  Serial.print("RMS velocity\t");
  Serial.print(mems_data.x.RmsVelocity,4);
  Serial.print("\t");
  Serial.print(mems_data.y.RmsVelocity,4);
  Serial.print("\t");
  Serial.print(mems_data.z.RmsVelocity,4);
  Serial.print("\r\n");
  Serial.print("RMS G\t");
  Serial.print(mems_data.x.RmsG);
  Serial.print("\t");
  Serial.print(mems_data.y.RmsG);
  Serial.print("\t");
  Serial.print(mems_data.z.RmsG);
  Serial.print("\r\n");


  Serial.print("Peak Frequency X: "); 
  Serial.print(mems_data.x.peakFreq); 
  Serial.print(" Hz");
  Serial.print("\t");
  Serial.println(mems_data.x.ipeak_Freq);
  Serial.print("Peak Frequency Y: "); 
  Serial.print(mems_data.y.peakFreq); 
  Serial.print(" Hz");
  Serial.print("\t");
  Serial.println(mems_data.y.ipeak_Freq);
  Serial.print("Peak Frequency Z: "); 
  Serial.print(mems_data.z.peakFreq); 
  Serial.print(" Hz");
  Serial.print("\t");
  Serial.println(mems_data.z.ipeak_Freq);
  Serial.print("\r\n");

  Serial.print("roll X: "); 
  Serial.println(mems_data.roll);
  Serial.print("\t");
  Serial.print("pitch: "); 
  Serial.println(mems_data.pitch);
  Serial.print("\t");
  Serial.print("yaw: "); 
  Serial.println(mems_data.yaw);
  Serial.print("\t");
  Serial.print("\r\n");
}

void Task_read_mems()
{
  if (millis() - mems_data.tick_task <= 10000) 
  {
    return;
  }

  if (mems_data.index < mems_samples)
  {
    accel.getEvent(&event);
    mems_data.x.gReal[mems_data.index]=event.acceleration.x-mems_data.x.ADJ_OFST;
    mems_data.x.gImag[mems_data.index]= 0.0;
    mems_data.y.gReal[mems_data.index]=event.acceleration.y-mems_data.y.ADJ_OFST;
    mems_data.y.gImag[mems_data.index]= 0.0;
 
    mems_data.z.gReal[mems_data.index]=event.acceleration.z-mems_data.z.ADJ_OFST;
    mems_data.z.gImag[mems_data.index]= 0.0;

    mems_data.x.Filter[mems_data.index] = mems_data.x.gReal[mems_data.index];
    mems_data.y.Filter[mems_data.index] = mems_data.y.gReal[mems_data.index];
    mems_data.z.Filter[mems_data.index] = mems_data.z.gReal[mems_data.index];
    if (mems_data.index > 0) {

        mems_data.x.Velocity[mems_data.index] = ((mems_data.x.Filter[mems_data.index]-mems_data.x.Filter[mems_data.index-1]) * (1.0000f / SAMPLE_RATE))*mems_data.x.Gain_velocity;
        mems_data.y.Velocity[mems_data.index] = ((mems_data.y.Filter[mems_data.index]-mems_data.y.Filter[mems_data.index-1]) * (1.0000f / SAMPLE_RATE))*mems_data.y.Gain_velocity;
        mems_data.z.Velocity[mems_data.index] = ((mems_data.z.Filter[mems_data.index]-mems_data.z.Filter[mems_data.index-1]) * (1.0000f / SAMPLE_RATE))*mems_data.z.Gain_velocity;
        
        mems_data.x.Velocity[mems_data.index] = mems_data.x.Velocity[mems_data.index-1]+((mems_data.x.Filter[mems_data.index]-mems_data.x.Filter[mems_data.index-1]) * (1.0 / SAMPLE_RATE));
        mems_data.y.Velocity[mems_data.index] = mems_data.y.Velocity[mems_data.index-1]+((mems_data.y.Filter[mems_data.index]-mems_data.y.Filter[mems_data.index-1]) * (1.0 / SAMPLE_RATE));
        mems_data.z.Velocity[mems_data.index] = mems_data.z.Velocity[mems_data.index-1]+((mems_data.z.Filter[mems_data.index]-mems_data.z.Filter[mems_data.index-1]) * (1.0 / SAMPLE_RATE));
    } else {
      mems_data.x.Velocity[mems_data.index] = 0.0;
      mems_data.y.Velocity[mems_data.index] = 0.0;
      mems_data.z.Velocity[mems_data.index] = 0.0;
    }
    mems_data.index++;
    mems_data.CAL_RDY=0;
  }
  else{
    mems_data.index=0;
    mems_data.CAL_RDY=1;
  }
    
  if(mems_data.CAL_RDY)
    {
      mems_data.CAL_RDY=0;
      // PrintRawData(mems_data.x.gReal,mems_data.y.gReal,mems_data.z.gReal, mems_samples);
      calculate_RMS_velocity();
      calculate_RMS_g();
      // calculate_rotation(&mems_data);
      // Serial.println("Calculate FFT");
      //if( (mems_data.x.RmsVelocity>=0.008)||(mems_data.y.RmsVelocity>=0.008)||(mems_data.z.RmsVelocity>=0.008))
      //  {
        Process_FFT();
      //  }
      // Print_FFT();
      mems_data.tick_task =millis();
    }
    
}


double calculate_rms_value(double *raw_data,int index)
{
  double buff_raw=0;
   for (int i = 0; i < index; i++)
   {
    buff_raw+=(*raw_data)*(*raw_data);
   }
   return(sqrt(buff_raw/index));
}

void calculate_RMS_velocity()
{
  mems_data.x.sumSqrVelocity=0;
  mems_data.y.sumSqrVelocity=0;
  mems_data.z.sumSqrVelocity=0;
  for (int i = 0; i < mems_samples; i++) {
    mems_data.x.sumSqrVelocity+=mems_data.x.Velocity[i]*mems_data.x.Velocity[i];
    mems_data.y.sumSqrVelocity+=mems_data.y.Velocity[i]*mems_data.y.Velocity[i];
    mems_data.z.sumSqrVelocity+=mems_data.z.Velocity[i]*mems_data.z.Velocity[i];
  }
  mems_data.x.RmsVelocity=sqrt(mems_data.x.sumSqrVelocity/mems_samples);
  mems_data.y.RmsVelocity=sqrt(mems_data.y.sumSqrVelocity/mems_samples);
  mems_data.z.RmsVelocity=sqrt(mems_data.z.sumSqrVelocity/mems_samples);
}

void calculate_RMS_g()
{
  mems_data.x.sumsqrG=0;
  mems_data.y.sumsqrG=0;
  mems_data.z.sumsqrG=0;
  for (int i = 0; i < mems_samples; i++) {
    mems_data.x.sumsqrG+=mems_data.x.gReal[i]*mems_data.x.gReal[i];
    mems_data.y.sumsqrG+=mems_data.y.gReal[i]*mems_data.y.gReal[i];
    mems_data.z.sumsqrG+=mems_data.z.gReal[i]*mems_data.z.gReal[i];
  }
  mems_data.x.RmsG=sqrt(mems_data.x.sumsqrG/mems_samples);
  mems_data.y.RmsG=sqrt(mems_data.y.sumsqrG/mems_samples);
  mems_data.z.RmsG=sqrt(mems_data.z.sumsqrG/mems_samples);
}

void calculate_rotation(virbration_mems *mems)
{
  mems->pitch = atan2(mems->y.RmsG, sqrt(mems->x.RmsG * mems->x.RmsG  + mems->z.RmsG  * mems->z.RmsG )) * 180.0 / PI;
  mems->roll = atan2(-mems->x.RmsG, sqrt(mems->y.RmsG * mems->y.RmsG + mems->z.RmsG * mems->z.RmsG)) * 180.0 / PI;
  mems->yaw= atan2(sqrt(mems->x.RmsG * mems->x.RmsG + mems->y.RmsG * mems->y.RmsG), mems->z.RmsG) * 180.0 / PI;
}

void Task_adxl345_mems()
{
  switch(st01.mems_cmd)
  {
    default:
    case 0:   // operate read 
      Task_read_mems();
      break;
    case 1:   //calibrate operate
      st01.mems_cmd=0;
      break;
  }
}


