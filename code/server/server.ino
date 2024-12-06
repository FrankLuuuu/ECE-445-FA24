#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <SPI.h>
#include <Ethernet.h>
#include <math.h>

Adafruit_MCP4725 dac;

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAF, 0x07, 0x68 };
IPAddress ip(192, 168, 1, 2);
EthernetServer server(502);

// Three-phase input pins
// Phase A: Voltage on A0, Current on A1
// Phase B: Voltage on A2, Current on A3
// Phase C: Voltage on A4, Current on A5
const int voltagePins[3] = {A0, A2, A4};
const int currentPins[3] = {A1, A3, A5};

const float voltageStepUpRatio = 100.0;  
const float currentStepUpRatio = 101.52284;  
const float voltageCalibration = 1.0;
const float currentCalibration = 1.0;
const float voltageOffset = 409.2; 
const float currentOffset = 503.4; 

#define TCA9548A_ADDRESS 0x70

// Function to select which TCA9548A channel (0-7)
void selectTCA9548AChannel(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(TCA9548A_ADDRESS);
  Wire.write(1 << channel);  
  Wire.endTransmission();
}

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  
  Wire.begin();               

  Serial.print("Ethernet server started at IP address: ");
  Serial.println(Ethernet.localIP());
  
  // Initialize each DAC by selecting the channel and calling dac.begin
  for (int channel = 0; channel < 6; channel++) {
    selectTCA9548AChannel(channel);  
    dac.begin(0x62); // each channel presumably has a MCP4725 at 0x62
  }
}

void loop() {
  int numSamples = 200;

  float voltageSum[3] = {0,0,0};
  float currentSum[3] = {0,0,0};
  float powerSum[3]   = {0,0,0};

  // Sample each phase
  for (int i = 0; i < numSamples; i++) {
    for (int ph = 0; ph < 3; ph++) {
      float voltageSample = analogRead(voltagePins[ph]) - voltageOffset;
      float currentSample = analogRead(currentPins[ph]) - currentOffset;

      float voltage = voltageSample * (5.0 / 1023.0) * voltageCalibration * voltageStepUpRatio;
      float current = currentSample * (5.0 / 1023.0) * currentCalibration * currentStepUpRatio;

      voltageSum[ph] += (voltage * voltage);
      currentSum[ph] += (current * current);
      powerSum[ph]   += (voltage * current);
    }
    delayMicroseconds(100);
  }

  // Calculate RMS and power values per phase
  float voltageRMS[3], currentRMS[3], realPower[3], apparentPower[3], reactivePower[3], powerFactor[3];

  for (int ph = 0; ph < 3; ph++) {
    voltageRMS[ph] = sqrt(voltageSum[ph] / numSamples);
    currentRMS[ph] = sqrt(currentSum[ph] / numSamples);
    realPower[ph]  = powerSum[ph] / numSamples;
    apparentPower[ph] = voltageRMS[ph] * currentRMS[ph];

    if (apparentPower[ph] == 0) {
      powerFactor[ph] = 0.0;
      reactivePower[ph] = 0.0;
    } else {
      powerFactor[ph] = realPower[ph] / apparentPower[ph];
      reactivePower[ph] = sqrt((apparentPower[ph]*apparentPower[ph]) - (realPower[ph]*realPower[ph]));
    }
  }

  // Print per-phase info
  for (int ph = 0; ph < 3; ph++) {
    Serial.print("Phase ");
    Serial.print(ph+1);
    Serial.println(":");
    Serial.print("  Voltage RMS: "); Serial.print(voltageRMS[ph]); Serial.println(" V");
    Serial.print("  Current RMS: "); Serial.print(currentRMS[ph]); Serial.println(" A");
    Serial.print("  Real Power: "); Serial.print(realPower[ph]); Serial.println(" W");
    Serial.print("  Apparent Power: "); Serial.print(apparentPower[ph]); Serial.println(" VA");
    Serial.print("  Reactive Power: "); Serial.print(reactivePower[ph]); Serial.println(" VAR");
    Serial.print("  Power Factor: "); Serial.println(powerFactor[ph]);
    Serial.println("");
  }

  // Calculate total system values
  float totalRealPower = realPower[0] + realPower[1] + realPower[2];
  float totalApparentPower = apparentPower[0] + apparentPower[1] + apparentPower[2];
  float totalReactivePower = reactivePower[0] + reactivePower[1] + reactivePower[2];
  float totalPowerFactor = (totalApparentPower == 0) ? 0 : (totalRealPower / totalApparentPower);

  Serial.println("Total System:");
  Serial.print("  Real Power: "); Serial.print(totalRealPower); Serial.println(" W");
  Serial.print("  Apparent Power: "); Serial.print(totalApparentPower); Serial.println(" VA");
  Serial.print("  Reactive Power: "); Serial.print(totalReactivePower); Serial.println(" VAR");
  Serial.print("  Power Factor: "); Serial.println(totalPowerFactor);
  Serial.println("");

  // Ethernet communication (as before)
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    while (client.connected()) {
      if (client.available()) {
        byte request[12];
        client.read(request, sizeof(request));

        // Scale and send a basic response similar to original code, but now for totals only (example)
        int16_t realP = (int16_t)totalRealPower;
        int16_t appP  = (int16_t)totalApparentPower;
        int16_t reacP = (int16_t)totalReactivePower;

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

        response[9] = highByte(realP);    
        response[10] = lowByte(realP);
        response[11] = highByte(appP);    
        response[12] = lowByte(appP);
        response[13] = highByte(reacP);   
        response[14] = lowByte(reacP);

        client.write(response, sizeof(response));
        client.stop();
      }
    }
    Serial.println("Client disconnected");
  }

  // Output to DACs
  // Channel mapping (0-based):
  // 0: Voltage Phase A
  // 1: Voltage Phase B
  // 2: Voltage Phase C
  // 3: Current Phase A
  // 4: Current Phase B
  // 5: Current Phase C

  // Convert each RMS to 0-3V:
  // Voltage output: voltageRMS/160 => 3 V max at 480V RMS
  // Current output: currentRMS/50  => 3 V max at 150A RMS
  
  // DAC code = (outputVolts/5.0)*4095 since DAC full-scale is 5V.
  for (int ph = 0; ph < 3; ph++) {
    float vOut = voltageRMS[ph] / 160.0; // 0-3 V
    uint16_t dacCode = (uint16_t)((vOut / 5.0) * 4095.0);
    selectTCA9548AChannel(ph);
    dac.setVoltage(dacCode, false);
    Serial.print("Voltage Phase "); Serial.print(ph+1); Serial.print(" DAC Output: "); 
    Serial.print(vOut, 3); Serial.println(" V");

    float iOut = currentRMS[ph] / 50.0;  // 0-3 V
    dacCode = (uint16_t)((iOut / 5.0) * 4095.0);
    selectTCA9548AChannel(ph+3);
    dac.setVoltage(dacCode, false);
    Serial.print("Current Phase "); Serial.print(ph+1); Serial.print(" DAC Output: "); 
    Serial.print(iOut, 3); Serial.println(" V");
  }

  delay(1000);
}
