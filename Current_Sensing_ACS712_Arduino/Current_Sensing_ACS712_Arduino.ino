/*
===============================================================================
Title:        Current_Sensing_ACS712
Date:         2026-02-24
Author:       Alejandro Alonso Puig + ChatGPT
License:      MIT License (short version: free to use, modify and distribute)
-------------------------------------------------------------------------------
Description:
This program performs a proof-of-concept test for the ACS712 5A current sensor
using an Arduino Uno.

Workflow:
1) Automatically calibrates the zero-current offset at startup.
2) Continuously reads the current using averaged ADC samples.
3) Converts ADC readings into amperes.
4) Sends raw ADC value, voltage and calculated current to the PC via Serial.

Designed for laboratory characterization using an electronic load.
===============================================================================
*/

#include <Arduino.h>

// --------------------------- USER CONFIGURATION -----------------------------

#define ACS_PIN        A0          // Analog pin connected to ACS712 OUT
#define NUM_SAMPLES    50          // Number of ADC samples per reading (noise reduction)
#define ADC_SCALE      1023.0      // 10-bit ADC resolution (0–1023)
#define VREF           5.0         // Arduino Uno default analog reference voltage (Volts)

// Sensitivity for ACS712 5A version = 185 mV/A = 0.185 V/A
#define SENSITIVITY    0.185       

// --------------------------- GLOBAL VARIABLES -------------------------------

int zeroOffset = 0;                // ADC value corresponding to 0A (calibrated at startup)

// --------------------------- FUNCTION PROTOTYPES ----------------------------

int calibrateZero();
float readCurrentDC();
float readAveragedADC();

// ---------------------------------------------------------------------------
// SETUP
// ---------------------------------------------------------------------------

void setup() {

  Serial.begin(115200);
  delay(1000);  // Allow serial monitor to stabilize

  Serial.println("===============================================");
  Serial.println("ACS712 5A Calibration and Measurement Program");
  Serial.println("Calibrating zero current offset...");
  Serial.println("Please ensure NO current is flowing.");
  Serial.println("===============================================");

  delay(3000);  // Time to ensure no load connected

  zeroOffset = calibrateZero();

  Serial.print("Zero offset (ADC units): ");
  Serial.println(zeroOffset);

  Serial.println("Calibration complete.");
  Serial.println("Starting current measurements...\n");
}

// ---------------------------------------------------------------------------
// LOOP
// ---------------------------------------------------------------------------

void loop() {

  float adcValue = readAveragedADC();  // Read averaged ADC value
  float voltage  = adcValue * (VREF / ADC_SCALE);  // Convert ADC to Volts

  // Compute current:
  // 1) Subtract zero offset
  // 2) Convert ADC units to voltage
  // 3) Divide by sensitivity (V/A)
  float current = (adcValue - zeroOffset) * (VREF / ADC_SCALE) / SENSITIVITY;

  Serial.print("ADC: ");
  Serial.print(adcValue, 2);

  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V");

  Serial.print(" | Current: ");
  Serial.print(current, 3);
  Serial.println(" A");

  delay(500);   // Update rate (2 readings per second)
}

// ---------------------------------------------------------------------------
// FUNCTION: calibrateZero
// Description:
// Reads multiple ADC samples with zero current and computes average offset.
// This value corresponds to Vcc/2 (nominally 2.5V) but measured precisely.
// ---------------------------------------------------------------------------

int calibrateZero() {

  long sum = 0;

  for (int i = 0; i < 100; i++) {   // 100 samples for stable calibration
    sum += analogRead(ACS_PIN);
    delay(5);                      // Small delay to reduce correlation
  }

  int offset = sum / 100;
  return offset;
}

// ---------------------------------------------------------------------------
// FUNCTION: readAveragedADC
// Description:
// Reads NUM_SAMPLES ADC values and returns their average.
// This reduces noise and improves measurement stability.
// ---------------------------------------------------------------------------

float readAveragedADC() {

  long sum = 0;

  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(ACS_PIN);
  }

  return (float)sum / NUM_SAMPLES;
}

// ---------------------------------------------------------------------------
// FUNCTION: readCurrentDC
// Description:
// Alternative single-call current computation function.
// Not used directly in loop(), but available for modular integration.
// ---------------------------------------------------------------------------

float readCurrentDC() {

  float adcValue = readAveragedADC();
  float current = (adcValue - zeroOffset) * (VREF / ADC_SCALE) / SENSITIVITY;
  return current;
}
