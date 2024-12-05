#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <SPI.h>
#include <Ethernet.h>

Adafruit_MCP4725 dac;

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAF, 0x07, 0x68 };
IPAddress ip(192, 168, 1, 2);
EthernetServer server(502);

const int voltagePin = A0; // Analog pin for voltage sensor
const int currentPin = A1; // Analog pin for current sensor

const float voltageStepUpRatio = 100.0;  
const float currentStepUpRatio = 101.52284;  
const float voltageCalibration = 1.0;
const float currentCalibration = 1.0;
const float voltageOffset = 409.2; 
const float currentOffset = 503.4; 

#define TCA9548A_ADDRESS 0x70

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
    dac.begin(0x62);                 
  }
}

void loop() {
  float voltageSum = 0;
  float currentSum = 0;
  float powerSum = 0;
  int numSamples = 200;

  int rawVoltageReading = analogRead(voltagePin);
  int rawCurrentReading = analogRead(currentPin);

  for (int i = 0; i < numSamples; i++) {
    float voltageSample = analogRead(voltagePin) - voltageOffset;
    float currentSample = analogRead(currentPin) - currentOffset;

    float voltage = voltageSample * (5.0 / 1023.0) * voltageCalibration * voltageStepUpRatio;
    float current = currentSample * (5.0 / 1023.0) * currentCalibration * currentStepUpRatio;

    voltageSum += voltage * voltage;
    currentSum += current * current;
    powerSum += voltage * current;

    delayMicroseconds(100);
  }

  float voltageRMS = sqrt(voltageSum / numSamples);
  float currentRMS = sqrt(currentSum / numSamples);
  float realPower = powerSum / numSamples;
  float apparentPower = voltageRMS * currentRMS;
  float reactivePower = sqrt(apparentPower * apparentPower - realPower * realPower);
  float powerFactor = realPower / apparentPower;

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
  Serial.print(powerFactor);
  Serial.println("");

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

  // Add DAC output functionality
  outputToDACs(currentRMS);  // testing with 150A current 

  delay(1000);
}

// Function to output voltage to six DACs via the TCA9548A multiplexer
void outputToDACs(float currentRMS) {
  // Calculate DAC output value based on currentRMS
  uint16_t dacValue = (currentRMS / 50.0) * 4095 / 5.0;  // Scale for 12-bit DAC (0–4095) with 5V reference
  dacValue = 0;

  // Output the different voltage to each DAC through the TCA9548A
  for (int channel = 0; channel < 6; channel++) {
    selectTCA9548AChannel(channel);  // Select each channel
    if (dacValue == 0)
      dac.setVoltage(dacValue, false);
    else
      dac.setVoltage(dacValue - 13, false); // Set DAC output without EEPROM save
    Serial.print("DAC Channel ");
    Serial.print(channel);
    Serial.print(" Output: ");
    Serial.println((float)dacValue * 5.0 / 4095.0, 2); // Print approximate output voltage
    dacValue += 410;  // Increment to set different voltage for each DAC (approx. 0 to 3V)
  }
}
