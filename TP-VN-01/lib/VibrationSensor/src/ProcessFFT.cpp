
#include <Arduino.h>
#include <Wire.h>
#include <arduinoFFT.h>
#include <mems.h>
#include "ProessFFT.h"

virbration_mems mems_data;
ArduinoFFT<float> FFT_X = ArduinoFFT<float>(mems_data.x.gReal, mems_data.x.gImag, NUM_SAMPLES, SAMPLE_RATE);
ArduinoFFT<float> FFT_Y = ArduinoFFT<float>(mems_data.y.gReal, mems_data.y.gImag, NUM_SAMPLES, SAMPLE_RATE);
ArduinoFFT<float> FFT_Z = ArduinoFFT<float>(mems_data.z.gReal, mems_data.z.gImag, NUM_SAMPLES, SAMPLE_RATE);

// ArduinoFFT<float> FFT_X = ArduinoFFT<float>(vn_mems.mems_data.x.gReal, vn_mems.mems_data.x.gImag, vn_mems.mems_samples, SAMPLE_RATE);
// ArduinoFFT<float> FFT_Y = ArduinoFFT<float>(vn_mems.mems_data.y.gReal, vn_mems.mems_data.y.gImag, vn_mems.mems_samples, SAMPLE_RATE);
// ArduinoFFT<float> FFT_Z = ArduinoFFT<float>(vn_mems.mems_data.z.gReal, vn_mems.mems_data.z.gImag, vn_mems.mems_samples, SAMPLE_RATE);


void update_top_peaks(float* peakFreqs, float* peakValues, int* peakIndices, float value, float freq, int index) {
    for (int i = 0; i < NUM_PEAKS; i++) {
        if (value > peakValues[i]) {
            // Shift the lower peaks down to make space for the new peak
            for (int j = NUM_PEAKS - 1; j > i; j--) {
                peakValues[j] = peakValues[j - 1];
                peakFreqs[j] = peakFreqs[j - 1];
                peakIndices[j] = peakIndices[j - 1];
            }
            // Insert the new peak
            peakValues[i] = value;
            peakFreqs[i] = freq;
            peakIndices[i] = index;
            break;
        }
    }
}

const float WINDOW_CORRECTION = 1.85f;

void Process_FFT(virbration_mems *pmems,int mems_samples)
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


    // pmems->x.gFFT[0]=pmems->x.gReal[0];
    // pmems->y.gFFT[0]=pmems->y.gReal[0];
    // pmems->z.gFFT[0]=pmems->z.gReal[0];
    // pmems->x.gFFT[1]=pmems->x.gReal[1];
    // pmems->y.gFFT[1]=pmems->y.gReal[1];
    // pmems->z.gFFT[1]=pmems->z.gReal[1];

    // Initialize peak frequencies and values to zero
    for (int i = 0; i < NUM_PEAKS; i++) {
        pmems->x.peakFreq[i] = 0;
        pmems->x.peakValue[i] = 0;
        pmems->x.ipeak_Freq[i] = 0;

        pmems->y.peakFreq[i] = 0;
        pmems->y.peakValue[i] = 0;
        pmems->y.ipeak_Freq[i] = 0;

        pmems->z.peakFreq[i] = 0;
        pmems->z.peakValue[i] = 0;
        pmems->z.ipeak_Freq[i] = 0;
    }

    for (int i = 0; i < (mems_samples >> 1); i++) 
      {
      double freq = (i * SAMPLE_RATE) / mems_samples;
      
      pmems->x.gFFT[i]= (2.0 * pmems->x.gReal[i]) / (NUM_SAMPLES);
      pmems->y.gFFT[i]= (2.0 * pmems->y.gReal[i]) / (NUM_SAMPLES);
      pmems->z.gFFT[i] = (2.0 * pmems->z.gReal[i]) / (NUM_SAMPLES);
    //   pmems->z.gFFT[i]=pmems->z.gReal[i];


        // Update the top 10 peaks for x-axis
        update_top_peaks(pmems->x.peakFreq, pmems->x.peakValue, pmems->x.ipeak_Freq, pmems->x.gFFT[i], freq, i);

        // Update the top 10 peaks for y-axis
        update_top_peaks(pmems->y.peakFreq, pmems->y.peakValue, pmems->y.ipeak_Freq, pmems->y.gFFT[i], freq, i);

        // Update the top 10 peaks for z-axis
        update_top_peaks(pmems->z.peakFreq, pmems->z.peakValue, pmems->z.ipeak_Freq, pmems->z.gFFT[i], freq, i);

      }
}