#include <SPI.h>
#include <Ethernet.h>
#include <math.h>  // Include the math library for arccosine

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAF, 0x07, 0x68 };
IPAddress ip(192, 168, 1, 2);
EthernetServer server(502);

// Three-phase pins
// Phase 1: Voltage = A0, Current = A1
// Phase 2: Voltage = A2, Current = A3
// Phase 3: Voltage = A4, Current = A5
const int voltagePins[3] = {A0, A2, A4}; 
const int currentPins[3] = {A1, A3, A5};

// Calibration and scaling factors (same for all phases in this example)
const float voltageStepUpRatio = 100.0;      // Voltage step-up ratio
const float currentStepUpRatio = 101.52284;  // Current step-up ratio
const float voltageCalibration = 1.018;      // Adjust this for voltage calibration
const float currentCalibration = 1.012;      // Adjust this for current calibration

// Offsets for each phase (if different per channel, adjust accordingly)
const float voltageOffset = 409.2; // ADC value for 2V DC offset (example)
const float currentOffset = 503.4; // ADC value for 2.462V DC offset (example)

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.print("Ethernet server started at IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  int numSamples = 200;

  // Arrays to accumulate sums per phase
  float voltageSum[3] = {0,0,0};
  float currentSum[3] = {0,0,0};
  float powerSum[3]   = {0,0,0};

  // Zero-crossing detection variables per phase
  unsigned long voltageZeroCrossTime[3] = {0,0,0};
  unsigned long currentZeroCrossTime[3] = {0,0,0};
  bool voltageCrossed[3] = {false,false,false};
  bool currentCrossed[3] = {false,false,false};

  // Sampling
  for (int i = 0; i < numSamples; i++) {
    for (int ph = 0; ph < 3; ph++) {
      int rawVoltageReading = analogRead(voltagePins[ph]);
      int rawCurrentReading = analogRead(currentPins[ph]);

      float voltageSample = rawVoltageReading - voltageOffset;
      float currentSample = rawCurrentReading - currentOffset;

      float voltage = voltageSample * (5.0 / 1023.0) * voltageCalibration * voltageStepUpRatio;
      float current = currentSample * (5.0 / 1023.0) * currentCalibration * currentStepUpRatio;

      voltageSum[ph] += (voltage * voltage);
      currentSum[ph] += (current * current);
      powerSum[ph]   += (voltage * current);

      // Zero-crossing detection for phase ph
      if (voltageSample >= 10 && !voltageCrossed[ph]) {
        voltageZeroCrossTime[ph] = micros();
        voltageCrossed[ph] = true;
      }
      if (currentSample >= 10 && !currentCrossed[ph]) {
        currentZeroCrossTime[ph] = micros();
        currentCrossed[ph] = true;
      }

      if (voltageSample < -10) voltageCrossed[ph] = false;
      if (currentSample < -10) currentCrossed[ph] = false;
    }
    delayMicroseconds(50);
  }

  // Now compute per-phase values
  float voltageRMS[3], currentRMS[3], realPower[3], apparentPower[3], powerFactor[3], phaseAngleDeg[3], reactivePower[3];
  String powerFactorType[3];

  for (int ph = 0; ph < 3; ph++) {
    voltageRMS[ph] = sqrt(voltageSum[ph] / numSamples);
    currentRMS[ph] = sqrt(currentSum[ph] / numSamples);
    realPower[ph]  = powerSum[ph] / numSamples;
    apparentPower[ph] = voltageRMS[ph] * currentRMS[ph];

    float pf = 0;
    if (apparentPower[ph] != 0) {
      pf = realPower[ph] / apparentPower[ph];
    }

    // Determine leading or lagging
    long phaseTimeDiff = (long)currentZeroCrossTime[ph] - (long)voltageZeroCrossTime[ph];
    // Correct phase wrapping (assuming 60 Hz, adjust if needed)
    float cycleTime = 1000000.0 / 60.0; // ~16666.7 microseconds per cycle
    if (phaseTimeDiff > (cycleTime / 2)) {
      phaseTimeDiff -= (long)cycleTime;
    } else if (phaseTimeDiff < -(cycleTime / 2)) {
      phaseTimeDiff += (long)cycleTime;
    }

    float phaseAngleRad = acos(abs(pf));
    powerFactorType[ph] = "Lagging";
    if (phaseTimeDiff < 0) {
      // Current leads voltage
      powerFactorType[ph] = "Leading";
      phaseAngleRad = -phaseAngleRad;
    }

    phaseAngleDeg[ph] = phaseAngleRad * (180.0 / M_PI);
    reactivePower[ph] = apparentPower[ph] * sin(phaseAngleRad);

    powerFactor[ph] = pf; // store signed PF for now

    // Print to serial
    Serial.print("Phase ");
    Serial.print(ph+1);
    Serial.println(":");
    Serial.print("  Voltage RMS: ");
    Serial.print(voltageRMS[ph]);
    Serial.println(" V");
    Serial.print("  Current RMS: ");
    Serial.print(currentRMS[ph]);
    Serial.println(" A");
    Serial.print("  Real Power: ");
    Serial.print(realPower[ph]);
    Serial.println(" W");
    Serial.print("  Apparent Power: ");
    Serial.print(apparentPower[ph]);
    Serial.println(" VA");
    Serial.print("  Reactive Power: ");
    Serial.print(reactivePower[ph]);
    Serial.println(" VAR");
    Serial.print("  Power Factor: ");
    Serial.print(abs(powerFactor[ph]));
    Serial.print(" (");
    Serial.print(powerFactorType[ph]);
    Serial.print("), Phase Angle: ");
    Serial.print(phaseAngleDeg[ph]);
    Serial.println(" degrees");
  }

  // Compute totals
  float totalRealPower = realPower[0] + realPower[1] + realPower[2];
  float totalApparentPower = apparentPower[0] + apparentPower[1] + apparentPower[2];
  float totalReactivePower = reactivePower[0] + reactivePower[1] + reactivePower[2];

  float totalPF = 0;
  if (totalApparentPower != 0) {
    totalPF = totalRealPower / totalApparentPower;
  }

  // For total PF sign determination, we can use a heuristic:
  // If the sum of phases has more leading or lagging phases, we can decide sign.
  // For simplicity, assume if total PF > 0 => Lagging, < 0 => Leading
  // Actually total PF should be based on sign of reactive (if needed).
  // Here we’ll assume the sign of totalPF determined by the sign of totalReactivePower:
  // if totalReactivePower < 0 => leading, else lagging.
  String totalPFType = "Lagging";
  if (totalReactivePower < 0) {
    totalPFType = "Leading";
  }

  Serial.println("Total System:");
  Serial.print("  Real Power: ");
  Serial.print(totalRealPower);
  Serial.println(" W");
  Serial.print("  Apparent Power: ");
  Serial.print(totalApparentPower);
  Serial.println(" VA");
  Serial.print("  Reactive Power: ");
  Serial.print(totalReactivePower);
  Serial.println(" VAR");
  Serial.print("  Power Factor: ");
  Serial.print(abs(totalPF));
  Serial.print(" (");
  Serial.print(totalPFType);
  Serial.println(")");

  // Prepare data for sending
  // We will send:
  // For each phase: Real (int), Apparent (int), Reactive (int), PF*1000 (signed int)
  // Then total: Real, Apparent, Reactive, PF*1000 (signed)
  // That's 4 registers per phase * 3 phases = 12 registers, plus 4 for total = 16 registers.
  // Each register is 2 bytes. Total data bytes = 32.
  // Make sure to scale and cast properly.

  // Helper lambda to convert float to int16 scaled
  auto toInt16 = [](float value) {
    int val = (int)round(value);
    if (val > 32767) val = 32767;
    if (val < -32768) val = -32768;
    return (int16_t)val;
  };

  auto toPFInt16 = [](float pf, String type) {
    // pf is between -1 and 1
    // If leading, we make pf negative, else positive
    float scaledPF = abs(pf) * 1000.0;
    if (type == "Leading") scaledPF = -scaledPF;
    int val = (int)round(scaledPF);
    if (val > 32767) val = 32767;
    if (val < -32768) val = -32768;
    return (int16_t)val;
  };

  // Construct response
  // We'll mimic Modbus:
  // Transaction ID: request[0..1]
  // Protocol ID: request[2..3]
  // Length: after we know how many bytes we'll send
  // Unit ID: request[6]
  // Function code: 0x03
  // Byte count: 32 (16 registers * 2)
  // Then the registers

  EthernetClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    while (client.connected()) {
      if (client.available()) {
        byte request[12];
        client.read(request, sizeof(request));

        // Prepare data array (32 data bytes + 9 header bytes)
        // Total length = 9 (header) + 32 (data) = 41 bytes
        byte response[41];
        response[0] = request[0];
        response[1] = request[1];
        response[2] = 0x00; // Protocol high
        response[3] = 0x00; // Protocol low
        response[4] = 0x00; // Length high
        response[5] = 0x23; // Length low (35 decimal + 6 header = 41 total?)
                           // Actually length field = number of bytes after this field: 
                           // Unit ID (1) + Function(1) + ByteCount(1) + Data(32) = 35 bytes total after these two length bytes
                           // So length should be 35 decimal = 0x23
        response[6] = request[6]; // Unit ID
        response[7] = 0x03;       // Function code
        response[8] = 32;         // Byte count (32 bytes of register data)

        // Fill registers
        // Order: Phase1(Real,Apparent,Reactive,PF), Phase2(...), Phase3(...), Total(...)
        int16_t regs[16];
        int idx = 0;
        for (int ph = 0; ph < 3; ph++) {
          regs[idx++] = toInt16(realPower[ph]);
          regs[idx++] = toInt16(apparentPower[ph]);
          regs[idx++] = toInt16(reactivePower[ph]);
          regs[idx++] = toPFInt16(powerFactor[ph], powerFactorType[ph]);
        }

        // Total
        regs[idx++] = toInt16(totalRealPower);
        regs[idx++] = toInt16(totalApparentPower);
        regs[idx++] = toInt16(totalReactivePower);
        regs[idx++] = toPFInt16(totalPF, totalPFType);

        // Convert regs to response bytes
        int byteIndex = 9;
        for (int i = 0; i < 16; i++) {
          response[byteIndex++] = highByte(regs[i]);
          response[byteIndex++] = lowByte(regs[i]);
        }

        client.write(response, sizeof(response));
        client.stop();
      }
    }
    Serial.println("Client disconnected");
  }

  delay(1000);
}
