#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoModbus.h> // Use ArduinoModbus library

// Step-up ratios for current and voltage (based on your step-down circuits)
const float voltageStepUpRatio = 20.0; // Adjust this based on your step-down ratio for voltage
const float currentStepUpRatio = 10.0; // Adjust this based on your step-down ratio for current

// Ethernet and Modbus setup
byte mac[] = { 0xA8, 0x61, 0x0A, 0xAF, 0x07, 0x68 };
IPAddress ip(192, 168, 1, 2); // Set your IP address

// Test values for voltage and current
float testVoltage = 2.5; // Simulated stepped-down voltage in volts
float testCurrent = 0.5; // Simulated stepped-down current in amps

void setup() {
  Serial.begin(9600);

  // Initialize Ethernet
  Ethernet.begin(mac, ip);

  // Start Modbus TCP server
  if (!ModbusTCPServer.begin()) {
    Serial.println("Failed to start Modbus TCP Server!");
    while (1); // Stop if server fails to start
  }

  // Configure 3 holding registers (for real, apparent, and reactive power)
  ModbusTCPServer.configureHoldingRegisters(0, 3);
  Serial.println("Modbus TCP Server started");
}

void loop() {
  // Simulate voltage and current values
  float simulatedVoltage = testVoltage * voltageStepUpRatio; // Step up to reflect actual high voltage
  float simulatedCurrent = testCurrent * currentStepUpRatio; // Step up to reflect actual high current

  // Calculate power values
  float realPower = simulatedVoltage * simulatedCurrent * 0.8; // Assuming a power factor of 0.8
  float apparentPower = simulatedVoltage * simulatedCurrent;
  float reactivePower = sqrt(apparentPower * apparentPower - realPower * realPower);

  // Output to Serial for testing
  Serial.print("Simulated Real Power: ");
  Serial.print(realPower);
  Serial.println(" W");

  Serial.print("Simulated Apparent Power: ");
  Serial.print(apparentPower);
  Serial.println(" VA");

  Serial.print("Simulated Reactive Power: ");
  Serial.print(reactivePower);
  Serial.println(" VAR");

  // Write power values to Modbus holding registers
  ModbusTCPServer.holdingRegisterWrite(0, (int)realPower);       // Real Power
  ModbusTCPServer.holdingRegisterWrite(1, (int)apparentPower);   // Apparent Power
  ModbusTCPServer.holdingRegisterWrite(2, (int)reactivePower);   // Reactive Power

  // Delay for testing
  delay(1000);
}
