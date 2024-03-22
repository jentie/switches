import socket

# Define the UDP server parameters
UDP_IP = "127.0.0.1"
UDP_PORT = 12345

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

while True:
    try:
        # Receive data from the client
        data, addr = sock.recvfrom(1024)
        data = data.decode().strip()

        # Process the received data
        if data == "w 20 aa":
            print("20 aa")
            response = "ok"
        elif data == "1":
            print("on")
            response = "ok"
        elif data == "0":
            print("off")
            response = "ok"
        else:
            print("err")
            response = "err"

        # Send the response back to the client
        sock.sendto(response.encode(), addr)
    except KeyboardInterrupt:
        print("\nServer stopped by user.")
        break
    except Exception as e:
        print(f"Error: {e}")

# Close the socket
sock.close()
