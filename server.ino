#include <SPI.h>
#include <Ethernet.h>
#include <ModbusTCP.h>
#include <EmonLib.h> // Include EmonLib for power calculations

// Define pins for voltage and current sensors
const int voltagePin = A0;
const int currentPin = A1;

// Step-up ratios for current and voltage (based on your step-down circuits)
const float voltageStepUpRatio = 20.0; // Adjust based on your step-down ratio for voltage
const float currentStepUpRatio = 10.0; // Adjust based on your step-down ratio for current

// Calibration values for your sensors
float voltageCalibration = 234.26; // Adjust based on your voltage sensor and calibration
float currentCalibration = 111.1;  // Adjust based on your current sensor and calibration

// Create an instance of the EnergyMonitor class
EnergyMonitor emon1;

// Ethernet and Modbus setup
byte mac[] = { 0xA8, 0x61, 0x0A, 0xAF, 0x07, 0x68 };
IPAddress ip(192, 168, 1, 177); // Set your IP address
ModbusTCPServer modbusTCP;

void setup() {
  Serial.begin(9600);

  // Initialize the Ethernet and Modbus server
  Ethernet.begin(mac, ip);
  modbusTCP.begin();
  modbusTCP.configureHoldingRegisters(0, 3); // Set up 3 registers for real, apparent, and reactive power

  // Initialize EmonLib with calibration values
  emon1.voltage(voltagePin, voltageCalibration, 1.7); // Calibration for voltage sensor
  emon1.current(currentPin, currentCalibration);      // Calibration for current sensor
}

void loop() {
  // Measure voltage and current with EmonLib
  emon1.calcVI(20, 2000);  // 20 cycles at 50Hz or adjust for your region

  // Scale up the measured values based on the step-up ratios
  float actualVoltage = emon1.Vrms * voltageStepUpRatio;
  float actualCurrent = emon1.Irms * currentStepUpRatio;

  // Calculate power values
  float realPower = actualVoltage * actualCurrent * emon1.powerFactor;
  float apparentPower = actualVoltage * actualCurrent;
  float reactivePower = sqrt(apparentPower * apparentPower - realPower * realPower);

  // Output to Serial for debugging
  Serial.print("Real Power: ");
  Serial.print(realPower);
  Serial.println(" W");

  Serial.print("Apparent Power: ");
  Serial.print(apparentPower);
  Serial.println(" VA");

  Serial.print("Reactive Power: ");
  Serial.print(reactivePower);
  Serial.println(" VAR");

  // Write power values to Modbus holding registers
  modbusTCP.holdingRegisterWrite(0, (int)realPower);
  modbusTCP.holdingRegisterWrite(1, (int)apparentPower);
  modbusTCP.holdingRegisterWrite(2, (int)reactivePower);

  // Delay for the next measurement
  delay(1000);
}
