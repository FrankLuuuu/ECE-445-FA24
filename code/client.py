import socket
import struct
import time

# Arduino's IP address and port
ARDUINO_IP = '192.168.1.2'  # Adjust to your Arduino's IP
MODBUS_PORT = 502

def read_power_values():
    try:
        # Create a TCP socket connection
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect((ARDUINO_IP, MODBUS_PORT))

        # Construct a Modbus-like request
        # Transaction ID = 1
        # Protocol ID = 0
        # Length = 6
        # Unit ID = 1
        # Function = 0x03 (read)
        # Starting address = 0
        # Number of registers = 3 (originally, but the server just responds anyway)
        # The server code doesn't strictly parse these, it's a placeholder.
        request = struct.pack('>HHHBBHH', 1, 0, 6, 1, 0x03, 0, 3)

        # Send the request to the Arduino
        client.send(request)

        # Receive the response from the Arduino
        # We know the server sends a total of 41 bytes:
        # (Header of 9 bytes + 32 data bytes)
        response = client.recv(256)  # More than enough to hold 41 bytes

        # Close the client connection
        client.close()

        if len(response) < 41:
            print("Invalid response length")
            return

        # Parse the Modbus-like header
        # response format:
        # Transaction ID: 2 bytes
        # Protocol ID: 2 bytes
        # Length: 2 bytes
        # Unit ID: 1 byte
        # Function: 1 byte
        # Byte Count: 1 byte
        # Data: 32 bytes (16 registers)
        transaction_id, protocol_id, length = struct.unpack('>HHH', response[0:6])
        unit_id = response[6]
        function_code = response[7]
        byte_count = response[8]

        if byte_count != 32:
            print("Unexpected byte count:", byte_count)
            return

        # Extract the 16 registers (32 bytes)
        data = response[9:9+32]
        # Unpack as 16 signed 16-bit integers
        regs = struct.unpack('>16h', data)

        # Helper function to interpret PF
        def interpret_pf(pf_int):
            # pf_int is signed scaled by 1000
            pf = pf_int / 1000.0
            pf_type = "Lagging" if pf >= 0 else "Leading"
            return abs(pf), pf_type

        def print_phase_data(phase_number, offset):
            rP = regs[offset]       # Real Power (W)
            aP = regs[offset+1]     # Apparent Power (VA)
            reP = regs[offset+2]    # Reactive Power (VAR)
            pf_val, pf_type = interpret_pf(regs[offset+3])

            print(f"Phase {phase_number}:")
            print(f"  Real Power: {rP} W")
            print(f"  Apparent Power: {aP} VA")
            print(f"  Reactive Power: {reP} VAR")
            print(f"  Power Factor: {pf_val} ({pf_type})\n")

        # Phase 1 data
        print_phase_data(1, 0)
        # Phase 2 data
        print_phase_data(2, 4)
        # Phase 3 data
        print_phase_data(3, 8)

        # Total system data
        total_rP = regs[12]
        total_aP = regs[13]
        total_reP = regs[14]
        total_pf_val, total_pf_type = interpret_pf(regs[15])

        print("Total System:")
        print(f"  Real Power: {total_rP} W")
        print(f"  Apparent Power: {total_aP} VA")
        print(f"  Reactive Power: {total_reP} VAR")
        print(f"  Power Factor: {total_pf_val} ({total_pf_type})")

    except Exception as e:
        print(f"Error: {e}")

# Run the read loop
while True:
    read_power_values()
    time.sleep(1)  # Poll every second
