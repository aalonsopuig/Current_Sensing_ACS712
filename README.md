# Current_Sensing_ACS712

This project provides a proof-of-concept implementation for DC current measurement using the ACS712 Hall-effect current sensor module and an Arduino board.

The program performs an automatic zero-offset calibration at startup and continuously reads the sensor output to estimate DC current. The measured values (raw ADC, voltage and calculated current in amperes) are sent to the PC through the USB connection for monitoring and validation.

The implementation is intended for laboratory characterization, servo current analysis and protection experiments in robotics systems.

---

## Hardware Used

The project was tested with an ACS712 5A module (marked `ACS712ELC-05B`) powered at 5 V and connected to an Arduino Uno.

The ACS712 is a Hall-effect linear current sensor. Its output voltage is centered around Vcc/2 at zero current and varies proportionally to the current flowing through the primary conductor.

According to the datasheet, the typical sensitivity for the 5A version is approximately 185 mV/A. However, practical testing has shown that some modules may exhibit significantly different effective sensitivity. This may be due to manufacturing variations, clones, or board layout differences. For this reason, empirical calibration is strongly recommended.

---

## Wiring

Typical wiring configuration:

- `ACS712 Vcc` → Arduino `5V`
- `ACS712 GND` → Arduino `GND`
- `ACS712 OUT` → Arduino analog input (e.g. `A0`)
- Load connected in series through the module’s current terminals

The module must be inserted in series with the load so that the current to be measured flows through the internal conductor.

---

## Program Behaviour

At startup, the program performs a zero-current calibration. During this phase, no current should be flowing through the sensor. The average ADC value measured during this period is stored as the zero offset.

After calibration, the program continuously:

1. Reads multiple ADC samples.
2. Computes an average value.
3. Converts the ADC reading into voltage.
4. Converts the voltage difference relative to the zero offset into current using the configured sensitivity.
5. Sends the results to the PC via USB.

---

## Configuration Parameters

The following parameters can be adjusted in the code:

### `ACS_PIN`
Analog input used to read the sensor output.

### `NUM_SAMPLES`
Number of ADC samples used to compute each averaged reading.  
Increasing this value reduces noise but slows down response time.

### `VREF`
Reference voltage used by the ADC (typically 5.0 V for Arduino Uno).  
For improved accuracy, measure the real 5 V rail with a multimeter and update this value accordingly.

### `SENSITIVITY`
Sensor sensitivity in volts per ampere.  
Nominal value for ACS712 5A is 0.185 V/A, but empirical calibration may yield different values (for example ~0.40 V/A observed in testing). This constant should be adjusted based on real measurements.

---

## Recommended Analog Filtering

Although the program performs digital averaging, the ACS712 output can exhibit high-frequency noise due to switching loads such as servos.

For improved stability, it is recommended to add an RC low-pass filter between the sensor output and the Arduino ADC input:

- Series resistor: 1 kΩ to 4.7 kΩ  
- Capacitor from ADC pin to GND: 100 nF to 1 µF  

This forms a simple first-order low-pass filter that significantly improves measurement stability without excessively reducing response speed.

Avoid placing large capacitors directly on the sensor output without a series resistor, as this may stress the sensor output stage.

---

## Calibration Recommendation

Due to observed variability between modules, sensitivity should not be assumed from the datasheet alone.

Recommended calibration procedure:

1. Measure zero-current voltage.
2. Apply a known DC current measured with a reliable multimeter.
3. Compute effective sensitivity: Sensitivity = (V(I) − V(0)) / I
4. Update the `SENSITIVITY` constant accordingly.

This ensures accurate current estimation for protection and monitoring purposes.

---

## Intended Use

This program is suitable for:

- Servo current characterization
- Stall detection experiments
- Overcurrent protection development
- Robotic subsystem validation

It is not intended for precision metrology applications without additional calibration and filtering.
