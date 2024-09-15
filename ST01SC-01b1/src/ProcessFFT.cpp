
#include <Arduino.h>
#include <Wire.h>
#include "arduinoFFT.h"
#include "sensor_board.h"
#include "ProessFFT.h"

// ArduinoFFT<double> FFT_X = ArduinoFFT<double>(mems_data.x.gReal, mems_data.x.gImag, mems_samples, SAMPLE_RATE);
// ArduinoFFT<double> FFT_Y = ArduinoFFT<double>(mems_data.y.gReal, mems_data.y.gImag, mems_samples, SAMPLE_RATE);
// ArduinoFFT<double> FFT_Z = ArduinoFFT<double>(mems_data.z.gReal, mems_data.z.gImag, mems_samples, SAMPLE_RATE);

ArduinoFFT<float> FFT_X = ArduinoFFT<float>(mems_data.x.gReal, mems_data.x.gImag, mems_samples, SAMPLE_RATE);
ArduinoFFT<float> FFT_Y = ArduinoFFT<float>(mems_data.y.gReal, mems_data.y.gImag, mems_samples, SAMPLE_RATE);
ArduinoFFT<float> FFT_Z = ArduinoFFT<float>(mems_data.z.gReal, mems_data.z.gImag, mems_samples, SAMPLE_RATE);


void Process_FFT()
{
    FFT_X.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT_X.compute(FFT_FORWARD);
    FFT_X.complexToMagnitude();

    FFT_Y.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT_Y.compute(FFT_FORWARD);
    FFT_Y.complexToMagnitude();

    FFT_Z.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT_Z.compute(FFT_FORWARD);
    FFT_Z.complexToMagnitude();

    mems_data.x.peakFreq=0;
    mems_data.x.peakValue=0;
    mems_data.y.peakFreq=0;
    mems_data.y.peakValue=0;
    mems_data.z.peakFreq=0;
    mems_data.z.peakValue=0;

    mems_data.x.gFFT[0]=mems_data.x.gReal[0];
    mems_data.y.gFFT[0]=mems_data.y.gReal[0];
    mems_data.z.gFFT[0]=mems_data.z.gReal[0];
    mems_data.x.gFFT[1]=mems_data.x.gReal[1];
    mems_data.y.gFFT[1]=mems_data.y.gReal[1];
    mems_data.z.gFFT[1]=mems_data.z.gReal[1];

    for (int i = 2; i < (mems_samples >> 1); i++) 
      {
      double freq = (i * samplingFrequency) / mems_samples;
      mems_data.x.gFFT[i]=mems_data.x.gReal[i];
      mems_data.y.gFFT[i]=mems_data.y.gReal[i];
      mems_data.z.gFFT[i]=mems_data.z.gReal[i];

      if (mems_data.x.gReal[i] > mems_data.x.peakValue) {
          mems_data.x.peakValue = mems_data.x.gReal[i];
          mems_data.x.peakFreq = freq;
          mems_data.x.ipeak_Freq = i;
        }
      if (mems_data.y.gReal[i] > mems_data.y.peakValue) {
          mems_data.y.peakValue = mems_data.y.gReal[i];
          mems_data.y.peakFreq = freq; 
          mems_data.y.ipeak_Freq = i;
        }
      if (mems_data.z.gReal[i] > mems_data.z.peakValue) {
          mems_data.z.peakValue = mems_data.z.gReal[i];
          mems_data.z.peakFreq = freq; 
          mems_data.z.ipeak_Freq = i;
        }
      }
}