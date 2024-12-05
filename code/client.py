import socket
import struct
import time

# Arduino's IP address and port
ARDUINO_IP = '192.168.1.2'  # Set this to the IP of your Arduino Uno
MODBUS_PORT = 502

def read_power_values():
    try:
        # Create a TCP socket connection
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect((ARDUINO_IP, MODBUS_PORT))

        # Construct a basic Modbus-like request
        # Transaction ID (2 bytes), Protocol ID (2 bytes), Length (2 bytes), Unit ID (1 byte), Function Code (1 byte)
        request = struct.pack('>HHHBBHH', 1, 0, 6, 1, 0x03, 0, 3)  # Read 3 registers starting at address 0

        # Send the request to the Arduino
        client.send(request)

        # Receive the response from the Arduino
        response = client.recv(256)  # Read up to 256 bytes

        # Close the client connection
        client.close()

        # Parse the response
        if len(response) >= 16:  # Ensure response has enough bytes
            transaction_id, protocol_id, length, unit_id, function_code, byte_count = struct.unpack('>HHHBBB', response[:9])
            real_power = struct.unpack('>H', response[9:11])[0]
            apparent_power = struct.unpack('>H', response[11:13])[0]
            reactive_power = struct.unpack('>H', response[13:15])[0]
            power_factor_type = "Leading" if response[15] == 0x01 else "Lagging"
            
            print(f"Real Power: {real_power} W")
            print(f"Apparent Power: {apparent_power} VA")
            print(f"Reactive Power: {reactive_power} VAR")
            print(f"Power Factor: {real_power / apparent_power:.4f} ({power_factor_type})")

        else:
            print("Invalid response from Arduino")
    
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    while True:
        read_power_values()
        time.sleep(1)
