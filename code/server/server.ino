#include <SPI.h>
#include <Ethernet.h>
#include <math.h>  // Include the math library for arccosine

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAF, 0x07, 0x68 };
IPAddress ip(192, 168, 1, 2);
EthernetServer server(502);

// Three-phase input pins
// Phase A: Voltage on A0, Current on A1
// Phase B: Voltage on A2, Current on A3
// Phase C: Voltage on A4, Current on A5
const int voltagePins[3] = {A0, A2, A4};
const int currentPins[3] = {A1, A3, A5};

// Calibration factors
const float voltageStepUpRatio = 100.0;  
const float currentStepUpRatio = 101.52284;  
const float voltageCalibration = 1.018; 
const float currentCalibration = 1.012; 

// Offsets
const float voltageOffset = 409.2; 
const float currentOffset = 503.4; 

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.print("Ethernet server started at IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  int numSamples = 200;
  unsigned long zeroCrossTimesVoltage[3] = {0,0,0};
  unsigned long zeroCrossTimesCurrent[3] = {0,0,0};
  bool crossedVoltage[3] = {false,false,false};
  bool crossedCurrent[3] = {false,false,false};

  // Accumulators for each phase
  float voltageSum[3] = {0,0,0};
  float currentSum[3] = {0,0,0};
  float powerSum[3]   = {0,0,0};

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

      // Zero crossing detection
      if (voltageSample >= 10 && !crossedVoltage[ph]) {
        zeroCrossTimesVoltage[ph] = micros();
        crossedVoltage[ph] = true;
      }
      if (currentSample >= 10 && !crossedCurrent[ph]) {
        zeroCrossTimesCurrent[ph] = micros();
        crossedCurrent[ph] = true;
      }

      if (voltageSample < -10) crossedVoltage[ph] = false;
      if (currentSample < -10) crossedCurrent[ph] = false;
    }
    delayMicroseconds(50);
  }

  // Frequency assumption
  float freq = 60.0;
  float cycleTime = 1000000.0 / freq;

  float voltageRMS[3], currentRMS[3], realPower[3], apparentPower[3], powerFactor[3], reactivePower[3];
  String powerFactorType[3];

  for (int ph = 0; ph < 3; ph++) {
    voltageRMS[ph] = sqrt(voltageSum[ph] / numSamples);
    currentRMS[ph] = sqrt(currentSum[ph] / numSamples);
    realPower[ph]  = powerSum[ph] / numSamples;
    apparentPower[ph] = voltageRMS[ph] * currentRMS[ph];
    powerFactor[ph] = (apparentPower[ph] == 0) ? 0 : (realPower[ph] / apparentPower[ph]);

    long phaseTimeDiff = (long)zeroCrossTimesCurrent[ph] - (long)zeroCrossTimesVoltage[ph];
    // Correct for wrapping
    if (phaseTimeDiff > (long)(cycleTime/2)) {
      phaseTimeDiff -= (long)cycleTime;
    } else if (phaseTimeDiff < -(long)(cycleTime/2)) {
      phaseTimeDiff += (long)cycleTime;
    }

    float phaseAngleRad = acos(abs(powerFactor[ph]));
    powerFactorType[ph] = "Lagging";
    if (phaseTimeDiff < 0) {
      // Current leads voltage
      powerFactorType[ph] = "Leading";
      phaseAngleRad = -phaseAngleRad;
    }

    reactivePower[ph] = apparentPower[ph] * sin(phaseAngleRad);

    // Print per-phase info
    Serial.print("Phase ");
    Serial.print(ph+1);
    Serial.println(":");
    Serial.print("  Voltage RMS: "); Serial.print(voltageRMS[ph]); Serial.println(" V");
    Serial.print("  Current RMS: "); Serial.print(currentRMS[ph]); Serial.println(" A");
    Serial.print("  Real Power: "); Serial.print(realPower[ph]); Serial.println(" W");
    Serial.print("  Apparent Power: "); Serial.print(apparentPower[ph]); Serial.println(" VA");
    Serial.print("  Reactive Power: "); Serial.print(reactivePower[ph]); Serial.println(" VAR");
    Serial.print("  Power Factor: "); Serial.print(abs(powerFactor[ph]));
    Serial.print(" ("); Serial.print(powerFactorType[ph]); Serial.println(")");
    Serial.println("");
  }

  // Calculate totals
  float totalRealPower = realPower[0] + realPower[1] + realPower[2];
  float totalApparentPower = (apparentPower[0] + apparentPower[1] + apparentPower[2]);
  float totalPowerFactor = (totalApparentPower == 0) ? 0 : (totalRealPower / totalApparentPower);
  float totalReactivePower = reactivePower[0] + reactivePower[1] + reactivePower[2];

  // Determine total leading/lagging (simplistic approach)
  long sumPhaseDiff = ((long)zeroCrossTimesCurrent[0]- (long)zeroCrossTimesVoltage[0]) +
                      ((long)zeroCrossTimesCurrent[1]- (long)zeroCrossTimesVoltage[1]) +
                      ((long)zeroCrossTimesCurrent[2]- (long)zeroCrossTimesVoltage[2]);
  String totalPowerFactorType = (sumPhaseDiff < 0) ? "Leading" : "Lagging";

  Serial.println("Total System:");
  Serial.print("  Real Power: "); Serial.print(totalRealPower); Serial.println(" W");
  Serial.print("  Apparent Power: "); Serial.print(totalApparentPower); Serial.println(" VA");
  Serial.print("  Reactive Power: "); Serial.print(totalReactivePower); Serial.println(" VAR");
  Serial.print("  Power Factor: "); Serial.print(abs(totalPowerFactor));
  Serial.print(" ("); Serial.print(totalPowerFactorType); Serial.println(")");
  Serial.println("");

  // Sending Data via Ethernet
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    while (client.connected()) {
      if (client.available()) {
        byte request[12];
        client.read(request, sizeof(request));

        // We'll scale:
        // Real Power, Apparent Power, Reactive Power by /10 before sending.
        // PF *1000 as before.
        // Lead/Lag: 1 or 0.

        auto packPhaseData = [&](int16_t *arr, int startIndex, float rp, float ap, float rq, float pf, String pftype) {
          int16_t realPowerInt     = (int16_t)(rp / 10.0);
          int16_t apparentPowerInt = (int16_t)(ap / 10.0);
          int16_t reactivePowerInt = (int16_t)(rq / 10.0);
          int16_t pfInt            = (int16_t)(abs(pf) * 1000.0);
          int16_t leadLagInt       = (pftype == "Leading") ? 1 : 0;

          arr[startIndex]   = realPowerInt;
          arr[startIndex+1] = apparentPowerInt;
          arr[startIndex+2] = reactivePowerInt;
          arr[startIndex+3] = pfInt;
          arr[startIndex+4] = leadLagInt;
        };

        int16_t data[20];
        // Phase A
        packPhaseData(data, 0,  realPower[0], apparentPower[0], reactivePower[0], powerFactor[0], powerFactorType[0]);
        // Phase B
        packPhaseData(data, 5,  realPower[1], apparentPower[1], reactivePower[1], powerFactor[1], powerFactorType[1]);
        // Phase C
        packPhaseData(data, 10, realPower[2], apparentPower[2], reactivePower[2], powerFactor[2], powerFactorType[2]);
        // Total
        packPhaseData(data, 15, totalRealPower, totalApparentPower, totalReactivePower, totalPowerFactor, totalPowerFactorType);

        // Modbus-like response framing
        // 20 registers * 2 bytes = 40 bytes data
        // Byte count = 40, plus function and unit ID, total length field
        byte response[49];
        response[0] = request[0];
        response[1] = request[1];
        response[2] = 0x00;
        response[3] = 0x00;
        response[4] = highByte(43); // 3 + 40 = 43 bytes after this header
        response[5] = lowByte(43);
        response[6] = request[6]; // Unit ID
        response[7] = 0x03;       // Function code
        response[8] = 40;         // Byte count for 20 registers

        int pos = 9;
        for (int i = 0; i < 20; i++) {
          response[pos++] = highByte(data[i]);
          response[pos++] = lowByte(data[i]);
        }

        client.write(response, sizeof(response));
        client.stop();
      }
    }
    Serial.println("Client disconnected");
  }

  delay(1000);
}
