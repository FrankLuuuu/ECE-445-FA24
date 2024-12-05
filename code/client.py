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
        request = struct.pack('>HHHBBHH', 1, 0, 6, 1, 0x03, 0, 18)  # Read 18 bytes starting at address 0

        # Send the request to the Arduino
        client.send(request)

        # Receive the response from the Arduino
        response = client.recv(256)  # Expecting 32 bytes based on server code

        # Close the client connection
        client.close()

        # Parse the response
        if len(response) >= 32:  # Ensure response has exactly 32 bytes
            transaction_id, protocol_id, length, unit_id, function_code, byte_count = struct.unpack('>HHHBBB', response[:9])

            # Each phase's power values (real power, apparent power, and reactive power)
            power_data = []
            for phase in range(3):
                base_index = 9 + phase * 6
                real_power = struct.unpack('>H', response[base_index + 0:base_index + 2])[0]
                apparent_power = struct.unpack('>H', response[base_index + 2:base_index + 4])[0]
                reactive_power = struct.unpack('>H', response[base_index + 4:base_index + 6])[0]

                power_data.append({
                    'real_power': real_power,
                    'apparent_power': apparent_power,
                    'reactive_power': reactive_power,
                    'power_factor': real_power / apparent_power if apparent_power != 0 else 0
                })

            # Print the results for all phases
            for phase_num, data in enumerate(power_data, start=1):
                print(f"Phase {phase_num}:")
                print(f"  Real Power: {data['real_power']} W")
                print(f"  Apparent Power: {data['apparent_power']} VA")
                print(f"  Reactive Power: {data['reactive_power']} VAR")
                print(f"  Power Factor: {data['power_factor']:.4f}")

            # Calculate total power (sum of all phases)
            total_real_power = sum(phase['real_power'] for phase in power_data)
            total_apparent_power = sum(phase['apparent_power'] for phase in power_data)
            total_reactive_power = sum(phase['reactive_power'] for phase in power_data)
            total_power_factor = total_real_power / total_apparent_power if total_apparent_power != 0 else 0

            print("\nTotal System Power Data:")
            print(f"  Total Real Power: {total_real_power} W")
            print(f"  Total Apparent Power: {total_apparent_power} VA")
            print(f"  Total Reactive Power: {total_reactive_power} VAR")
            print(f"  Total Power Factor: {total_power_factor:.4f}")

        else:
            print("Invalid response from Arduino")
    
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    while True:
        read_power_values()
        time.sleep(1)
