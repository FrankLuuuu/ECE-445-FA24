#include <SPI.h>
#include <Ethernet.h>
#include <math.h>  // Include the math library for arccosine

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAF, 0x07, 0x68 };
IPAddress ip(192, 168, 1, 2);
EthernetServer server(502);

const int voltagePins[3] = {A0, A1, A2}; // Analog pins for three-phase voltage
const int currentPins[3] = {A3, A4, A5}; // Analog pins for three-phase current

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
  float voltageSums[3] = {0, 0, 0};
  float currentSums[3] = {0, 0, 0};
  float powerSums[3] = {0, 0, 0};
  int numSamples = 200;

  unsigned long voltageZeroCrossTime[3] = {0, 0, 0};
  unsigned long currentZeroCrossTime[3] = {0, 0, 0};
  bool voltageCrossed[3] = {false, false, false};
  bool currentCrossed[3] = {false, false, false};

  // Sampling loop for three-phase AC voltage and current
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
      voltageSums[phase] += voltage * voltage;
      currentSums[phase] += current * current;
      powerSums[phase] += voltage * current;

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

      delayMicroseconds(50);
    }
  }

  // Calculate RMS values, power, and power factor for each phase
  float voltageRMS[3], currentRMS[3], realPower[3], apparentPower[3], reactivePower[3], powerFactor[3];
  for (int phase = 0; phase < 3; phase++) {
    voltageRMS[phase] = sqrt(voltageSums[phase] / numSamples);
    currentRMS[phase] = sqrt(currentSums[phase] / numSamples);
    realPower[phase] = powerSums[phase] / numSamples;
    apparentPower[phase] = voltageRMS[phase] * currentRMS[phase];
    powerFactor[phase] = realPower[phase] / apparentPower[phase];
  }

  // Calculate phase angle and determine if the current is leading or lagging for each phase
  long phaseTimeDiff[3];
  String powerFactorType[3];
  for (int phase = 0; phase < 3; phase++) {
    phaseTimeDiff[phase] = currentZeroCrossTime[phase] - voltageZeroCrossTime[phase];

    if (phaseTimeDiff[phase] > (1000000 / 60 / 2)) { // Half a cycle
      phaseTimeDiff[phase] -= (1000000 / 60);       // Subtract one full cycle
    } else if (phaseTimeDiff[phase] < -(1000000 / 60 / 2)) {
      phaseTimeDiff[phase] += (1000000 / 60);       // Add one full cycle
    }

    // Calculate phase angle using arccosine of the power factor
    float phaseAngleRad = acos(abs(powerFactor[phase]));
    powerFactorType[phase] = (phaseTimeDiff[phase] < 0) ? "Leading" : "Lagging";

    if (phaseTimeDiff[phase] < 0) {
      phaseAngleRad = -phaseAngleRad;  // Negate the phase angle for leading power factor
    }

    // Reactive power for each phase
    reactivePower[phase] = apparentPower[phase] * sin(phaseAngleRad);
  }

  // Calculate total power for all three phases
  float totalRealPower = 0, totalApparentPower = 0, totalReactivePower = 0;
  for (int phase = 0; phase < 3; phase++) {
    totalRealPower += realPower[phase];
    totalApparentPower += apparentPower[phase];
    totalReactivePower += reactivePower[phase];
  }

  // Print calculated values for each phase
  for (int phase = 0; phase < 3; phase++) {
    Serial.print("Phase ");
    Serial.print(phase + 1);
    Serial.print(" - Voltage RMS: ");
    Serial.print(voltageRMS[phase]);
    Serial.print(" V, Current RMS: ");
    Serial.print(currentRMS[phase]);
    Serial.print(" A, Real Power: ");
    Serial.print(realPower[phase]);
    Serial.print(" W, Apparent Power: ");
    Serial.print(apparentPower[phase]);
    Serial.print(" VA, Reactive Power: ");
    Serial.print(reactivePower[phase]);
    Serial.print(" VAR, Power Factor: ");
    Serial.print(abs(powerFactor[phase]));
    Serial.print(" (");
    Serial.print(powerFactorType[phase]);
    Serial.println(")");

    Serial.println("------------");
  }

  // Print total power values
  Serial.print("Total Real Power: ");
  Serial.print(totalRealPower);
  Serial.println(" W");

  Serial.print("Total Apparent Power: ");
  Serial.print(totalApparentPower);
  Serial.println(" VA");

  Serial.print("Total Reactive Power: ");
  Serial.print(totalReactivePower);
  Serial.println(" VAR");

  // Ethernet communication
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    while (client.connected()) {
      if (client.available()) {
        byte request[12];
        client.read(request, sizeof(request));

        byte response[33];  // Corrected response length (9 bytes header + 24 bytes data)

        // Fill response header
        response[0] = request[0];
        response[1] = request[1];
        response[2] = 0x00;
        response[3] = 0x00;
        response[4] = 0x00;
        response[5] = 0x09;  // Length of the remaining response (9 bytes header)
        response[6] = request[6];
        response[7] = 0x03;
        response[8] = 0x18;  // Byte count for data (24 bytes for 3 phases)

        // Fill in phase data for response (Real Power, Apparent Power, Reactive Power, Power Factor)
        for (int phase = 0; phase < 3; phase++) {
          response[9 + phase * 8] = highByte((int)realPower[phase]);      // Real Power high byte
          response[10 + phase * 8] = lowByte((int)realPower[phase]);      // Real Power low byte
          response[11 + phase * 8] = highByte((int)apparentPower[phase]); // Apparent Power high byte
          response[12 + phase * 8] = lowByte((int)apparentPower[phase]); // Apparent Power low byte
          response[13 + phase * 8] = highByte((int)reactivePower[phase]);// Reactive Power high byte
          response[14 + phase * 8] = lowByte((int)reactivePower[phase]); // Reactive Power low byte
          response[15 + phase * 8] = highByte((int)(powerFactor[phase] * 1000));  // Power Factor high byte (scaled)
          response[16 + phase * 8] = lowByte((int)(powerFactor[phase] * 1000));   // Power Factor low byte (scaled)
        }

        client.write(response, sizeof(response));
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
  delay(1000);
}