from pymodbus.client.sync import ModbusTcpClient
import time

# Arduino IP address and port (default Modbus TCP port is 502)
ARDUINO_IP = '192.168.1.177'
MODBUS_PORT = 502

# Connect to the Modbus server on the Arduino
client = ModbusTcpClient(ARDUINO_IP, port=MODBUS_PORT)

if client.connect():
    print("Connected to Arduino Modbus Server")
else:
    print("Failed to connect to Arduino Modbus Server")
    exit()

try:
    while True:
        # Read holding registers (3 registers in this example)
        response = client.read_holding_registers(0, 3)

        if response.isError():
            print("Error reading from Modbus server")
        else:
            # Extract data from response
            real_power = response.registers[0]
            apparent_power = response.registers[1]
            reactive_power = response.registers[2]

            # Display the data
            print(f"Real Power: {real_power} W")
            print(f"Apparent Power: {apparent_power} VA")
            print(f"Reactive Power: {reactive_power} VAR")
            print("-" * 30)

        # Wait before the next read
        time.sleep(1)

except KeyboardInterrupt:
    print("Program interrupted by user")

finally:
    client.close()
    print("Modbus client disconnected")
