#include "mems.h"
#include <ProessFFT.h>



mems::mems(unsigned long tkinterval):_interval(tkinterval)
{
    accel = Adafruit_ADXL345_Unified(12345);
}

void mems::set_mems_data(virbration_mems *psource)
{
    pmems_data=psource;
}

void mems::displaySensorDetails(void)
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

void mems::displayDataRate(void)
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

void mems::displayRange(void)
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


void mems::init_adxl3245_module()
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
  mems_g_range=8;
  mems_sampling_rage=SAMPLE_RATE;
  mems_task_period=1000/mems_sampling_rage;
  mems_THRESH_ACT=2;   //62.5x2 mg
  // Set activity detection with threshold (value: 0-255 → scale to Gs)
  mems_INT_SOURCE=accel.readRegister(ADXL345_REG_INT_SOURCE);
  accel.writeRegister(ADXL345_REG_THRESH_ACT, 3);  // ~1.25G
  accel.writeRegister(ADXL345_REG_ACT_INACT_CTL, 0xF0); // 1111 0000
//   accel.writeRegister(ADXL345_REG_ACT_INACT_CTL, 0x70); // 1111 0000 DC mode
  
  pmems_data->x.Gain_velocity=1;
  pmems_data->y.Gain_velocity=1;
  pmems_data->z.Gain_velocity=1;
  switch(mems_g_range)
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
  for (int i = 0; i < mems_samples; i++)
  {
    pmems_data->x.Velocity[i]=0;
    pmems_data->y.Velocity[i]=0;
    pmems_data->z.Velocity[i]=0;
  }

  if(mems_sampling_rage<=1600)
  {
    if(mems_sampling_rage<=800)
    {
        if(mems_sampling_rage<=400)
        {
            if(mems_sampling_rage<=200)
            {
                if(mems_sampling_rage<=100)
                {     accel.setDataRate(ADXL345_DATARATE_100_HZ);                }
                else
                {     accel.setDataRate(ADXL345_DATARATE_200_HZ);                }
            }
            else
                {       accel.setDataRate(ADXL345_DATARATE_400_HZ);            }
        }
        else
            {   accel.setDataRate(ADXL345_DATARATE_800_HZ);        }
    }
    else
    {       accel.setDataRate(ADXL345_DATARATE_1600_HZ);        }
  }
  else      //      set data rage 3200 Hz
    {     accel.setDataRate(ADXL345_DATARATE_3200_HZ);             }
//   accel.setDataRate(ADXL345_DATARATE_800_HZ);
  /* Display some basic information on this sensor */
  displaySensorDetails();
  /* Display additional settings (outside the scope of sensor_t) */
  displayDataRate();
  displayRange();
  // Enable activity interrupt on INT1
//   accel.writeRegister(ADXL345_REG_INT_ENABLE, 0x10); // Enable activity
  accel.writeRegister(ADXL345_REG_INT_MAP, 0x00);    // Activity -> INT1

  // Optional: Enable inactivity if you want to return to sleep later
  accel.writeRegister(ADXL345_REG_THRESH_INACT, 3); // ~0.625G
  accel.writeRegister(ADXL345_REG_TIME_INACT, 2);    // 2 seconds
  accel.writeRegister(ADXL345_REG_INT_ENABLE, 0x18); // Enable activity + inactivity
  motionDetected=true;
  Serial.printf("task period:%f\tsampling:%d\r\n",mems_task_period,mems_sampling_rage);
  taskmems.attach_ms(mems_task_period, periodicTaskWrapper, this);
}

void mems::periodicTaskWrapper(mems* instance) {
    instance->Task_read_mems();
}


double mems::calculate_rms_value(double *raw_data,int index)
{
  double buff_raw=0;
   for (int i = 0; i < index; i++)
   {
    buff_raw+=(*raw_data)*(*raw_data);
   }
   return(sqrt(buff_raw/index));
}

void mems::calculate_RMS_velocity()
{
  pmems_data->x.sumSqrVelocity=0;
  pmems_data->y.sumSqrVelocity=0;
  pmems_data->z.sumSqrVelocity=0;
  for (int i = 0; i < mems_samples; i++) {
    pmems_data->x.sumSqrVelocity+=pmems_data->x.Velocity[i]*pmems_data->x.Velocity[i];
    pmems_data->y.sumSqrVelocity+=pmems_data->y.Velocity[i]*pmems_data->y.Velocity[i];
    pmems_data->z.sumSqrVelocity+=pmems_data->z.Velocity[i]*pmems_data->z.Velocity[i];
  }
  pmems_data->x.RmsVelocity=sqrt(pmems_data->x.sumSqrVelocity/mems_samples);
  pmems_data->y.RmsVelocity=sqrt(pmems_data->y.sumSqrVelocity/mems_samples);
  pmems_data->z.RmsVelocity=sqrt(pmems_data->z.sumSqrVelocity/mems_samples);
}

void mems::calculate_RMS_g()
{
  pmems_data->x.sumsqrG=0;
  pmems_data->y.sumsqrG=0;
  pmems_data->z.sumsqrG=0;
  for (int i = 0; i < mems_samples; i++) {
    pmems_data->x.sumsqrG+=pmems_data->x.gReal[i]*pmems_data->x.gReal[i];
    pmems_data->y.sumsqrG+=pmems_data->y.gReal[i]*pmems_data->y.gReal[i];
    pmems_data->z.sumsqrG+=pmems_data->z.gReal[i]*pmems_data->z.gReal[i];
  }
  pmems_data->x.RmsG=sqrt(pmems_data->x.sumsqrG/mems_samples);
  pmems_data->y.RmsG=sqrt(pmems_data->y.sumsqrG/mems_samples);
  pmems_data->z.RmsG=sqrt(pmems_data->z.sumsqrG/mems_samples);
}

float mems::Calculate_Inclination(virbration_mems *mems)
{
    float Ax_f = (float)mems->x.gReal[pmems_data->index];
    float Ay_f = (float)mems->y.gReal[pmems_data->index];
    float Az_f = (float)mems->z.gReal[pmems_data->index];

    float magnitude = sqrt(Ax_f * Ax_f + Ay_f * Ay_f + Az_f * Az_f);

    // Protect against divide by zero
    if (magnitude == 0)
        return 0.0f;

    float inclination = acos(Az_f / magnitude);

    // Convert inclination from radians to degrees
    inclination = inclination * (180.0f / M_PI);

    return inclination;
}

void mems::calculate_rotation(virbration_mems *mems)
{
  float Ax = (float)mems->x.gReal[pmems_data->index];
  float Ay = (float)mems->y.gReal[pmems_data->index];
  float Az = (float)mems->z.gReal[pmems_data->index];

  // mems->pitch = atan2(mems->x.RmsG, sqrt(-mems->x.RmsG * mems->x.RmsG  + mems->z.RmsG  * mems->z.RmsG )) * 180.0 / PI;
  // mems->roll = atan2(-mems->x.RmsG, sqrt(mems->y.RmsG * mems->y.RmsG + mems->z.RmsG * mems->z.RmsG)) * 180.0 / PI;
  // mems->yaw= atan2(sqrt(mems->x.RmsG * mems->x.RmsG + mems->y.RmsG * mems->y.RmsG), mems->z.RmsG) * 180.0 / PI;
   // Pitch calculation
  // mems->pitch = atan2f(-Ax, sqrtf(Ay * Ay + Az * Az));
  mems->pitch = atan2f(Ay, sqrtf(Ax * Ax + Az * Az));
  mems->pitch = mems->pitch * (180.0f / M_PI); // Convert from radians to degrees

    // Roll calculation
  // mems->roll = atan2f(Ay, sqrtf(Ax * Ax + Az * Az));
  mems->roll = atan2f(-Ax, sqrtf(Ay * Ay + Az * Az));
  mems->roll = mems->roll * (180.0f / M_PI);  // Convert from radians to degrees

  mems->Inclination = Calculate_Inclination(mems);
}


void mems::Task_read_mems()
{
//   unsigned long currentMillis = millis();
//   if (currentMillis - _lastMillis < _interval) {
//     // Serial.print("wait sampling\r\n");
//       return;
//   }
if (motionDetected==false) 
    {
    return;
    }
if (millis() - pmems_data->tick_task <= 50) 
  {
    return;
  }
  
  if (pmems_data->index < mems_samples)
  {
    accel.getEvent(&event);
    pmems_data->x.gReal[pmems_data->index]=event.acceleration.x-pmems_data->x.ADJ_OFST;
    pmems_data->x.gRaw[pmems_data->index]=event.acceleration.x-pmems_data->x.ADJ_OFST;
    pmems_data->x.gImag[pmems_data->index]= 0.0;

    pmems_data->y.gReal[pmems_data->index]=event.acceleration.y-pmems_data->y.ADJ_OFST;
    pmems_data->y.gRaw[pmems_data->index]=event.acceleration.y-pmems_data->y.ADJ_OFST;
    pmems_data->y.gImag[pmems_data->index]= 0.0;

    pmems_data->z.gReal[pmems_data->index]=event.acceleration.z-pmems_data->z.ADJ_OFST;
    pmems_data->z.gRaw[pmems_data->index]=event.acceleration.z-pmems_data->z.ADJ_OFST;
    pmems_data->z.gImag[pmems_data->index]= 0.0;
    if( (pmems_data->x.gReal[pmems_data->index]>-0.03)&&(pmems_data->x.gReal[pmems_data->index]<0.03) )
      {
      pmems_data->x.gReal[pmems_data->index]=0;
      }
    if( (pmems_data->y.gReal[pmems_data->index]>-0.03)&&(pmems_data->y.gReal[pmems_data->index]<0.03) )
      {
      pmems_data->y.gReal[pmems_data->index]=0;
      }
    if( (pmems_data->z.gReal[pmems_data->index]>-0.03)&&(pmems_data->z.gReal[pmems_data->index]<0.03) )
      {
      pmems_data->z.gReal[pmems_data->index]=0;
      }

    pmems_data->x.Filter[pmems_data->index] = pmems_data->x.gReal[pmems_data->index];
    pmems_data->y.Filter[pmems_data->index] = pmems_data->y.gReal[pmems_data->index];
    pmems_data->z.Filter[pmems_data->index] = pmems_data->z.gReal[pmems_data->index];
    if (pmems_data->index > 0) {

        // pmems_data->x.Velocity[pmems_data->index] = ((pmems_data->x.Filter[pmems_data->index]-pmems_data->x.Filter[pmems_data->index-1]) * (1.0000f / SAMPLE_RATE))*pmems_data->x.Gain_velocity;
        // pmems_data->y.Velocity[pmems_data->index] = ((pmems_data->y.Filter[pmems_data->index]-pmems_data->y.Filter[pmems_data->index-1]) * (1.0000f / SAMPLE_RATE))*pmems_data->y.Gain_velocity;
        // pmems_data->z.Velocity[pmems_data->index] = ((pmems_data->z.Filter[pmems_data->index]-pmems_data->z.Filter[pmems_data->index-1]) * (1.0000f / SAMPLE_RATE))*pmems_data->z.Gain_velocity;
        
        pmems_data->x.Velocity[pmems_data->index] = pmems_data->x.Velocity[pmems_data->index-1]+((pmems_data->x.Filter[pmems_data->index]-pmems_data->x.Filter[pmems_data->index-1]) * (1.0 / SAMPLE_RATE))/2;
        pmems_data->y.Velocity[pmems_data->index] = pmems_data->y.Velocity[pmems_data->index-1]+((pmems_data->y.Filter[pmems_data->index]-pmems_data->y.Filter[pmems_data->index-1]) * (1.0 / SAMPLE_RATE))/2;
        pmems_data->z.Velocity[pmems_data->index] = pmems_data->z.Velocity[pmems_data->index-1]+((pmems_data->z.Filter[pmems_data->index]-pmems_data->z.Filter[pmems_data->index-1]) * (1.0 / SAMPLE_RATE))/2;

    } else {
      pmems_data->x.Velocity[pmems_data->index] = 0.0;
      pmems_data->y.Velocity[pmems_data->index] = 0.0;
      pmems_data->z.Velocity[pmems_data->index] = 0.0;
    }
    pmems_data->index++;
    pmems_data->CAL_RDY=0;
  }
  else{
    
    pmems_data->index=0;
    pmems_data->CAL_RDY=1;
      
  }
    
  if(pmems_data->CAL_RDY)
    {
      pmems_data->CAL_RDY=0;
    //   PrintRawData();
      calculate_RMS_velocity();
      calculate_RMS_g();
      calculate_rotation(pmems_data);

      Process_FFT(pmems_data,mems_samples);
      
      pmems_data->tick_task =millis();
      motionDetected=false;
      motion_weak_up=true;
      mems_INT_SOURCE=accel.readRegister(ADXL345_REG_INT_SOURCE);
      Print_FFT();
    //   _lastMillis = currentMillis;
    }
    
}

void mems::PrintRawData()
{
    int i;
    Serial.print("x\t\ty\t\tz\t\t\n\r");
    for(i=0;i<mems_samples;i++)
        {
            Serial.printf("%.03f\t\t%0.3f\t\t%0.3f\t\t\r\n",pmems_data->x.gReal[i],pmems_data->y.gReal[i],pmems_data->z.gReal[i]); 
        }
}

void mems::Print_FFT()
{
  Serial.printf("INT:%d\r\n",mems_INT_SOURCE);
  Serial.print("RMS velocity\t");
  Serial.print(pmems_data->x.RmsVelocity,4);
  Serial.print("\t");
  Serial.print(pmems_data->y.RmsVelocity,4);
  Serial.print("\t");
  Serial.print(pmems_data->z.RmsVelocity,4);
  Serial.print("\r\n");
  Serial.print("RMS G\t");
  Serial.print(pmems_data->x.RmsG);
  Serial.print("\t");
  Serial.print(pmems_data->y.RmsG);
  Serial.print("\t");
  Serial.print(pmems_data->z.RmsG);
  Serial.print("\r\n");


  Serial.print("Peak Frequency X: "); 
  Serial.print(pmems_data->x.peakFreq[0]); 
  Serial.print(" Hz");
  Serial.print("\t");
  Serial.println(pmems_data->x.ipeak_Freq[0]);
  Serial.print("Peak Frequency Y: "); 
  Serial.print(pmems_data->y.peakFreq[0]); 
  Serial.print(" Hz");
  Serial.print("\t");
  Serial.println(pmems_data->y.ipeak_Freq[0]);
  Serial.print("Peak Frequency Z: "); 
  Serial.print(pmems_data->z.peakFreq[0]); 
  Serial.print(" Hz");
  Serial.print("\t");
  Serial.println(pmems_data->z.ipeak_Freq[0]);
  Serial.print("\r\n");

  Serial.print("roll X: "); 
  Serial.println(pmems_data->roll);
//   Serial.print("\t");
  Serial.print("pitch: "); 
  Serial.println(pmems_data->pitch);
//   Serial.print("\t");
  Serial.print("yaw: "); 
  Serial.println(pmems_data->yaw);
  Serial.print("\t");
  Serial.print("\r\n");
}
