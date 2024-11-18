#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAF, 0x07, 0x68 };
IPAddress ip(192, 168, 1, 2);
EthernetServer server(502);

const int voltagePin = A0; // Analog pin for voltage sensor
const int currentPin = A1; // Analog pin for current sensor

const float voltageStepUpRatio = 20.0;  // Voltage step-up ratio
const float currentStepUpRatio = 10.0;  // Current step-up ratio
const float voltageCalibration = 234.26; // Adjust this for voltage calibration
const float currentCalibration = 111.1;  // Adjust this for current calibration

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
  float voltageOffset = 512; // Offset for AC signal (2.5V on a 10-bit ADC)
  float currentOffset = 512; // Offset for AC signal (2.5V on a 10-bit ADC)

  // Sampling loop for AC voltage and current
  for (int i = 0; i < numSamples; i++) {
    // Read and normalize voltage and current
    float voltageSample = analogRead(voltagePin) - voltageOffset;
    float currentSample = analogRead(currentPin) - currentOffset;

    // Convert to actual voltage/current by scaling
    float voltage = voltageSample * (5.0 / 1023.0) * voltageCalibration / voltageStepUpRatio;
    float current = currentSample * (5.0 / 1023.0) * currentCalibration / currentStepUpRatio;

    // Accumulate RMS values and instantaneous power
    voltageSum += voltage * voltage;
    currentSum += current * current;
    powerSum += voltage * current;

    delayMicroseconds(200); // Sampling delay (adjust based on your requirements)
  }

  // Calculate RMS values and power
  float voltageRMS = sqrt(voltageSum / numSamples);
  float currentRMS = sqrt(currentSum / numSamples);
  float realPower = powerSum / numSamples;
  float apparentPower = voltageRMS * currentRMS;
  float reactivePower = sqrt(apparentPower * apparentPower - realPower * realPower);

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
