import socket
import struct
import time

ARDUINO_IP = '192.168.1.2'
MODBUS_PORT = 502

def read_power_values():
    try:
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect((ARDUINO_IP, MODBUS_PORT))

        # Now we request 20 registers instead of 3
        # Transaction ID=1, Protocol ID=0, Length=6, Unit=1, Func=3, Start=0, Count=20
        request = struct.pack('>HHHBBHH', 1, 0, 6, 1, 0x03, 0, 20)
        client.send(request)

        response = client.recv(256)
        client.close()

        # Parse the response
        # response layout: 
        # 0-1: Transaction ID
        # 2-3: Protocol ID
        # 4-5: Length
        # 6: Unit ID
        # 7: Function code
        # 8: Byte count (should be 40)
        # 9 onwards: 20 registers * 2 bytes each
        if len(response) >= 49:
            data_bytes = response[9:]  # 40 bytes of data
            values = struct.unpack('>20h', data_bytes)

            # Helper to print phase data
            def print_phase(name, start_index):
                rp = values[start_index]
                ap = values[start_index+1]
                rq = values[start_index+2]
                pf_scaled = values[start_index+3]
                lead_lag = values[start_index+4]  # 0=Lagging, 1=Leading
                pf = pf_scaled / 1000.0
                pf_type = "Leading" if lead_lag == 1 else "Lagging"
                print(f"{name}:")
                print(f"  Real Power: {rp} W")
                print(f"  Apparent Power: {ap} VA")
                print(f"  Reactive Power: {rq} VAR")
                print(f"  Power Factor: {abs(pf)} ({pf_type})")

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
