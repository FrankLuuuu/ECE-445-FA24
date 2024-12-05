#include <SPI.h>
#include <Ethernet.h>
#include <math.h>  // Include the math library for arccosine

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAF, 0x07, 0x68 };
IPAddress ip(192, 168, 1, 2);
EthernetServer server(502);

// Pin assignments for 3-phase voltage and current
const int voltagePins[3] = { A0, A1, A2 }; // Voltage for Phase A, B, C
const int currentPins[3] = { A3, A4, A5 }; // Current for Phase A, B, C

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
  float voltageSum[3] = {0, 0, 0};
  float currentSum[3] = {0, 0, 0};
  float powerSum[3] = {0, 0, 0};
  int numSamples = 200;

  unsigned long voltageZeroCrossTime[3] = {0, 0, 0};
  unsigned long currentZeroCrossTime[3] = {0, 0, 0};
  bool voltageCrossed[3] = {false, false, false};
  bool currentCrossed[3] = {false, false, false};

  // Sampling loop for 3-phase AC voltage and current
  for (int i = 0; i < numSamples; i++) {
    for (int phase = 0; phase < 3; phase++) {
      int rawVoltageReading = analogRead(voltagePins[phase]);
      int rawCurrentReading = analogRead(currentPins[phase]);

      // Read and normalize voltage and current
      float voltageSample = rawVoltageReading - voltageOffset;
      float currentSample = rawCurrentReading - currentOffset;

      // Convert to actual voltage/current by scaling
      float voltage = voltageSample * (5.0 / 1023.0) * voltageCalibration * voltageStepUpRatio;
      float current = currentSample * (5.0 / 1023.0) * currentCalibration * currentStepUpRatio;

      // Accumulate RMS values and instantaneous power for each phase
      voltageSum[phase] += voltage * voltage;
      currentSum[phase] += current * current;
      powerSum[phase] += voltage * current;

      // Add hysteresis to zero-crossing detection for each phase
      if (voltageSample >= 10 && !voltageCrossed[phase]) {
        voltageZeroCrossTime[phase] = micros();
        voltageCrossed[phase] = true;
      }
      if (currentSample >= 10 && !currentCrossed[phase]) {
        currentZeroCrossTime[phase] = micros();
        currentCrossed[phase] = true;
      }

      // Reset crossing flags once signal goes below threshold
      if (voltageSample < -10) voltageCrossed[phase] = false;
      if (currentSample < -10) currentCrossed[phase] = false;
    }

    delayMicroseconds(50);
  }

  float voltageRMS[3], currentRMS[3], realPower[3], apparentPower[3], reactivePower[3], powerFactor[3];
  float totalRealPower = 0, totalApparentPower = 0, totalReactivePower = 0;

  // Calculate RMS values, power, and power factor for each phase
  for (int phase = 0; phase < 3; phase++) {
    voltageRMS[phase] = sqrt(voltageSum[phase] / numSamples);
    currentRMS[phase] = sqrt(currentSum[phase] / numSamples);
    realPower[phase] = powerSum[phase] / numSamples;
    apparentPower[phase] = voltageRMS[phase] * currentRMS[phase];
    powerFactor[phase] = realPower[phase] / apparentPower[phase];
    reactivePower[phase] = apparentPower[phase] * sqrt(1 - powerFactor[phase] * powerFactor[phase]);

    totalRealPower += realPower[phase];
    totalApparentPower += apparentPower[phase];
    totalReactivePower += reactivePower[phase];
  }

  // Calculate total power factor (based on total apparent and real power)
  float totalPowerFactor = totalRealPower / totalApparentPower;

  // Print individual phase data
  for (int phase = 0; phase < 3; phase++) {
    Serial.print("Phase ");
    Serial.print(phase + 1);
    Serial.print(" - Voltage RMS: ");
    Serial.print(voltageRMS[phase]);
    Serial.println(" V");
    
    Serial.print("Phase ");
    Serial.print(phase + 1);
    Serial.print(" - Current RMS: ");
    Serial.print(currentRMS[phase]);
    Serial.println(" A");
    
    Serial.print("Phase ");
    Serial.print(phase + 1);
    Serial.print(" - Real Power: ");
    Serial.print(realPower[phase]);
    Serial.println(" W");
    
    Serial.print("Phase ");
    Serial.print(phase + 1);
    Serial.print(" - Apparent Power: ");
    Serial.print(apparentPower[phase]);
    Serial.println(" VA");
    
    Serial.print("Phase ");
    Serial.print(phase + 1);
    Serial.print(" - Reactive Power: ");
    Serial.print(reactivePower[phase]);
    Serial.println(" VAR");
    
    Serial.print("Phase ");
    Serial.print(phase + 1);
    Serial.print(" - Power Factor: ");
    Serial.println(powerFactor[phase]);
  }

  // Print total system data
  Serial.println("Total System Power Data:");
  Serial.print("Total Real Power: ");
  Serial.print(totalRealPower);
  Serial.println(" W");
  
  Serial.print("Total Apparent Power: ");
  Serial.print(totalApparentPower);
  Serial.println(" VA");
  
  Serial.print("Total Reactive Power: ");
  Serial.print(totalReactivePower);
  Serial.println(" VAR");
  
  Serial.print("Total Power Factor: ");
  Serial.println(totalPowerFactor);

  // Ethernet communication
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    while (client.connected()) {
      if (client.available()) {
        byte request[12];
        client.read(request, sizeof(request));

        byte response[32];  // Adjusted response length
        response[0] = request[0];
        response[1] = request[1];
        response[2] = 0x00;
        response[3] = 0x00;
        response[4] = 0x00;
        response[5] = 0x0A;
        response[6] = request[6];
        response[7] = 0x03;
        response[8] = 0x12;  // Changed byte count to 18 for 3 phases

        // Add data for all 3 phases
        for (int phase = 0; phase < 3; phase++) {
          response[9 + phase * 6] = highByte((int)realPower[phase]);
          response[10 + phase * 6] = lowByte((int)realPower[phase]);
          response[11 + phase * 6] = highByte((int)apparentPower[phase]);
          response[12 + phase * 6] = lowByte((int)apparentPower[phase]);
          response[13 + phase * 6] = highByte((int)reactivePower[phase]);
          response[14 + phase * 6] = lowByte((int)reactivePower[phase]);
        }

        client.write(response, sizeof(response));
        client.stop();
      }
    }
    Serial.println("Client disconnected");
  }

  delay(1000);
}
