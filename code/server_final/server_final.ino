#include <SPI.h>
#include <Ethernet.h>
#include <math.h>  // Include the math library for arccosine

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAF, 0x07, 0x68 };
IPAddress ip(192, 168, 1, 2);
EthernetServer server(502);

const int voltagePin = A0; // Analog pin for voltage sensor
const int currentPin = A1; // Analog pin for current sensor

const float voltageStepUpRatio = 100.0;  // Voltage step-up ratio
const float currentStepUpRatio = 101.52284;  // Current step-up ratio
const float voltageCalibration = 1.018; // Adjust this for voltage calibration
const float currentCalibration = 1.012; // Adjust this for current calibration

const float voltageOffset = 409.2; // ADC value for 2V DC offset
const float currentOffset = 503.4; // ADC value for 2.462V DC offset

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.print("Ethernet server started at IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  float voltageSum = 0;
  float currentSum = 0;
  float powerSum = 0;
  int numSamples = 200;

  unsigned long voltageZeroCrossTime = 0;
  unsigned long currentZeroCrossTime = 0;
  bool voltageCrossed = false;
  bool currentCrossed = false;

  // Sampling loop for AC voltage and current
  for (int i = 0; i < numSamples; i++) {
    int rawVoltageReading = analogRead(voltagePin);
    int rawCurrentReading = analogRead(currentPin);

    // Print raw ADC readings to Serial Monitor
    // Serial.print("Raw Voltage Reading: ");
    // Serial.print(rawVoltageReading);
    // Serial.print(" | Raw Current Reading: ");
    // Serial.println(rawCurrentReading);

    // Read and normalize voltage and current
    float voltageSample = rawVoltageReading - voltageOffset;
    float currentSample = rawCurrentReading - currentOffset;

    // Convert to actual voltage/current by scaling
    float voltage = voltageSample * (5.0 / 1023.0) * voltageCalibration * voltageStepUpRatio;
    float current = currentSample * (5.0 / 1023.0) * currentCalibration * currentStepUpRatio;

    // Accumulate RMS values and instantaneous power
    voltageSum += voltage * voltage;
    currentSum += current * current;
    powerSum += voltage * current;

    // Add hysteresis to zero-crossing detection
    if (voltageSample >= 10 && !voltageCrossed) { // 10 ADC units as hysteresis
        voltageZeroCrossTime = micros();
        voltageCrossed = true;
    }
    if (currentSample >= 10 && !currentCrossed) { // 10 ADC units as hysteresis
        currentZeroCrossTime = micros();
        currentCrossed = true;
    }

    // Reset crossing flags once signal goes below threshold
    if (voltageSample < -10) voltageCrossed = false;
    if (currentSample < -10) currentCrossed = false;


    delayMicroseconds(50);
  }

  // Calculate RMS values and power
  float voltageRMS = sqrt(voltageSum / numSamples);
  float currentRMS = sqrt(currentSum / numSamples);
  float realPower = powerSum / numSamples;
  float apparentPower = voltageRMS * currentRMS;
  float powerFactor = realPower / apparentPower;

  long phaseTimeDiff = currentZeroCrossTime - voltageZeroCrossTime;
  // Correct phase wrapping
  if (phaseTimeDiff > (1000000 / 60 / 2)) { // Half a cycle (8.33 ms)
    phaseTimeDiff -= (1000000 / 60);       // Subtract one full cycle
  } else if (phaseTimeDiff < -(1000000 / 60 / 2)) {
    phaseTimeDiff += (1000000 / 60);       // Add one full cycle
  }
  Serial.print("Phase Diff: ");
  Serial.print(phaseTimeDiff);
  Serial.println("");

  // Calculate phase angle using arccosine of the power factor
  float phaseAngleRad = acos(abs(powerFactor));  // Phase angle in radians

  String powerFactorType = "Lagging";
  // If power factor is negative, the current is leading the voltage
  if (phaseTimeDiff < 0) {
    powerFactorType = "Leading";
    phaseAngleRad = -phaseAngleRad;  // Negate the phase angle for leading power factor
  }

  float phaseAngleDeg = phaseAngleRad * (180.0 / M_PI);  // Convert to degrees
  float reactivePower = apparentPower * sin(phaseAngleRad);

  // Calculate Reactive Power with the correct sign
  // if (powerFactorType == "Leading") {
  //   reactivePower = -reactivePower; // Negate reactive power for leading power factor
  // }

  // Print calculated values to Serial Monitor
  Serial.print("Voltage RMS: ");
  Serial.print(voltageRMS);
  Serial.println(" V");

  Serial.print("Current RMS: ");
  Serial.print(currentRMS);
  Serial.println(" A");

  Serial.print("Real Power: ");
  Serial.print(realPower);
  Serial.println(" W");

  Serial.print("Apparent Power: ");
  Serial.print(apparentPower);
  Serial.println(" VA");

  Serial.print("Reactive Power: ");
  Serial.print(reactivePower);
  Serial.println(" VAR");

  Serial.print("Power Factor: ");
  Serial.print(abs(powerFactor));
  Serial.print(" (");
  Serial.print(powerFactorType);
  Serial.print("), Phase Angle: ");
  Serial.print(phaseAngleDeg);
  Serial.println(" degrees");

  // Ethernet communication
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    while (client.connected()) {
      if (client.available()) {
        byte request[12];
        client.read(request, sizeof(request));

        byte response[15];
        response[0] = request[0];
        response[1] = request[1];
        response[2] = 0x00;
        response[3] = 0x00;
        response[4] = 0x00;
        response[5] = 0x09;
        response[6] = request[6];
        response[7] = 0x03;
        response[8] = 0x06;

        response[9] = highByte((int)realPower);      // Real Power high byte
        response[10] = lowByte((int)realPower);      // Real Power low byte
        response[11] = highByte((int)apparentPower); // Apparent Power high byte
        response[12] = lowByte((int)apparentPower);  // Apparent Power low byte
        response[13] = highByte((int)reactivePower); // Reactive Power high byte
        response[14] = lowByte((int)reactivePower);  // Reactive Power low byte

        client.write(response, sizeof(response));
        client.stop();
      }
    }
    Serial.println("Client disconnected");
  }

  delay(1000);
}
