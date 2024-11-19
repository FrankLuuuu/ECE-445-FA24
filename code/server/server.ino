#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAF, 0x07, 0x68 };
IPAddress ip(192, 168, 1, 2);
EthernetServer server(502);

const int voltagePin = A0; // Analog pin for voltage sensor
const int currentPin = A1; // Analog pin for current sensor

const float voltageStepUpRatio = 100.0;  // Voltage step-up ratio       // input = 120V
const float currentStepUpRatio = 101.52284;  // Current step-up ratio   // input = 150A
const float voltageCalibration = 1.018;//120; // Adjust this for voltage calibration
const float currentCalibration = 1.012;//150;  // Adjust this for current calibration

// Updated offsets based on actual DC offsets of the signals
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
  int numSamples = 200; // Number of samples per calculation

  int rawVoltageReading = analogRead(voltagePin);
  int rawCurrentReading = analogRead(currentPin);

  // Print raw ADC readings to Serial Monitor
  Serial.print("Raw Voltage Reading: ");
  Serial.print(rawVoltageReading);
  Serial.print(" | Raw Current Reading: ");
  Serial.println(rawCurrentReading);


  // if (voltageSample < 0 || currentSample < 0) {
  //   Serial.println("\n\n------------------------------------------------------------\n");
  //   Serial.println("input values too low!");
  //   Serial.println("\n------------------------------------------------------------\n\n");
  //   break;
  // }

  // Sampling loop for AC voltage and current
  for (int i = 0; i < numSamples; i++) {
    // Read and normalize voltage and current
    float voltageSample = analogRead(voltagePin) - voltageOffset;
    float currentSample = analogRead(currentPin) - currentOffset;

    // Convert to actual voltage/current by scaling
    float voltage = voltageSample * (5.0 / 1023.0) * voltageCalibration * voltageStepUpRatio;
    float current = currentSample * (5.0 / 1023.0) * currentCalibration * currentStepUpRatio;

    // Accumulate RMS values and instantaneous power
    voltageSum += voltage * voltage;
    currentSum += current * current;
    powerSum += voltage * current;

    delayMicroseconds(100); // Sampling delay (adjust based on your requirements)
  }

  // Calculate RMS values and power
  float voltageRMS = sqrt(voltageSum / numSamples);
  float currentRMS = sqrt(currentSum / numSamples);
  float realPower = powerSum / numSamples;
  float apparentPower = voltageRMS * currentRMS;
  float reactivePower = sqrt(apparentPower * apparentPower - realPower * realPower);
  float powerFactor = realPower / apparentPower;

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

  delay(1000);
}
