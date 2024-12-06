import socket
import struct
import time

ARDUINO_IP = '192.168.1.2'  # Arduino's IP
MODBUS_PORT = 502

def read_power_values():
    try:
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect((ARDUINO_IP, MODBUS_PORT))

        # Request 20 registers
        request = struct.pack('>HHHBBHH', 1, 0, 6, 1, 0x03, 0, 20)
        client.send(request)

        response = client.recv(256)
        client.close()

        if len(response) >= 49:
            data_bytes = response[9:]  # first 9 bytes are header
            values = struct.unpack('>20h', data_bytes)

            def print_phase(name, start_index):
                rp = values[start_index] * 10.0   # scaled up
                ap = values[start_index+1] * 10.0
                rq = values[start_index+2] * 10.0
                pf = values[start_index+3] / 1000.0
                lead_lag = values[start_index+4]
                if rp == 0 and ap == 0 and rq == 0 and pf == 0 and lead_lag == 0:
                    # This indicates no input
                    print(f"{name}:")
                    print("  No input detected.")
                    return

                pf_type = "Leading" if lead_lag == 1 else "Lagging"

                print(f"{name}:")
                print(f"  Real Power: {rp:.2f} W")
                print(f"  Apparent Power: {ap:.2f} VA")
                print(f"  Reactive Power: {rq:.2f} VAR")
                print(f"  Power Factor: {abs(pf):.3f} ({pf_type})")

            print_phase("Phase A", 0)
            print_phase("Phase B", 5)
            print_phase("Phase C", 10)
            print_phase("Total", 15)
        else:
            print("Invalid response length")

    except Exception as e:
        print(f"Error: {e}")

while True:
    read_power_values()
    time.sleep(1)
