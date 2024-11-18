#include <SPI.h>
#include <Ethernet.h>

// Ethernet configuration
byte mac[] = { 0xA8, 0x61, 0x0A, 0xAF, 0x07, 0x68 };
IPAddress ip(192, 168, 1, 2); // Set a static IP for the Arduino
EthernetServer server(502);   // Standard Modbus TCP port

// Step-up ratios for current and voltage (based on your step-down circuits)
const float voltageStepUpRatio = 20.0;
const float currentStepUpRatio = 10.0;

// Test values for voltage and current
float testVoltage = 2.5;
float testCurrent = 0.5;

void setup() {
  Serial.begin(9600);

  // Start Ethernet connection
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.print("Ethernet server started at IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Simulate stepped-up voltage and current values
  float simulatedVoltage = testVoltage * voltageStepUpRatio;
  float simulatedCurrent = testCurrent * currentStepUpRatio;

  // Calculate power values
  float realPower = simulatedVoltage * simulatedCurrent * 0.8; // Assuming a power factor of 0.8
  float apparentPower = simulatedVoltage * simulatedCurrent;
  float reactivePower = sqrt(apparentPower * apparentPower - realPower * realPower);

  // Listen for incoming client connections
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New client connected");

    // Wait for client data
    while (client.connected()) {
      if (client.available()) {
        // Read the client's request (dummy Modbus request)
        byte request[12];
        client.read(request, sizeof(request));

        // Prepare a Modbus-like response with all three power values
        byte response[15];
        
        // Initialize each part of the response array individually
        response[0] = request[0];                    // Transaction ID (byte 1)
        response[1] = request[1];                    // Transaction ID (byte 2)
        response[2] = 0x00;                          // Protocol ID (byte 1)
        response[3] = 0x00;                          // Protocol ID (byte 2)
        response[4] = 0x00;                          // Length (byte 1)
        response[5] = 0x09;                          // Length (byte 2 - 9 bytes following)
        response[6] = request[6];                    // Unit ID
        response[7] = 0x03;                          // Function Code (Read Holding Registers)
        response[8] = 0x06;                          // Byte count (6 bytes for 3 registers)

        // Split realPower, apparentPower, and reactivePower into high and low bytes
        response[9] = highByte((int)realPower);       // Real Power high byte
        response[10] = lowByte((int)realPower);       // Real Power low byte
        response[11] = highByte((int)apparentPower);  // Apparent Power high byte
        response[12] = lowByte((int)apparentPower);   // Apparent Power low byte
        response[13] = highByte((int)reactivePower);  // Reactive Power high byte
        response[14] = lowByte((int)reactivePower);   // Reactive Power low byte

        // Send response to client
        client.write(response, sizeof(response));
        client.stop(); // Close connection after sending response
      }
    }
    Serial.println("Client disconnected");
  }

  delay(1000); // Delay for testing purposes
}
