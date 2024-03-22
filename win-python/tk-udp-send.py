import socket
import time
import tkinter as tk
from tkinter import messagebox



root = tk.Tk()
root.title("UDP Client")
output_label = tk.Label(root, text=" ")
output_label.grid(row=6, columnspan=11)


def send_udp_message(message, server_ip, server_port):
    try:
        # Create a UDP socket
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # Send the message to the server
        client_socket.sendto(message.encode(), (server_ip, server_port))

        # Receive the response from the server
        response, _ = client_socket.recvfrom(1024)

        # Close the socket
        client_socket.close()

        return response.decode()
    except Exception as e:
        return str(e)


def send_R_message():
    response = send_udp_message("R", server_ip, server_port)
    output_label.config(text=f"response: {response}")
def send_r_message():
    response = send_udp_message("r", server_ip, server_port)
    output_label.config(text=f"response: {response}")

def send_G_message():
    response = send_udp_message("G", server_ip, server_port)
    output_label.config(text=f"response: {response}")
def send_g_message():
    response = send_udp_message("g", server_ip, server_port)
    output_label.config(text=f"response: {response}")

def send_B_message():
    response = send_udp_message("B", server_ip, server_port)
    output_label.config(text=f"response: {response}")
def send_b_message():
    response = send_udp_message("b", server_ip, server_port)
    output_label.config(text=f"response: {response}")

def send_W_message():
    response = send_udp_message("W", server_ip, server_port)
    output_label.config(text=f"response: {response}")
def send_w_message():
    response = send_udp_message("w", server_ip, server_port)
    output_label.config(text=f"response: {response}")


def send_1_message():
    response = send_udp_message("1", server_ip, server_port)
    output_label.config(text=f"response: {response}")

def send_0_message():
    response = send_udp_message("0", server_ip, server_port)
    output_label.config(text=f"response: {response}")

def send_v_message():
    response = send_udp_message("v", server_ip, server_port)
    output_label.config(text=f"response: {response}")

def send_V_message():
    response = send_udp_message("V", server_ip, server_port)
    output_label.config(text=f"response: {response}")

def send_Q_message():
    response = send_udp_message("?", server_ip, server_port)
    output_label.config(text=f"response: {response}")


def main():
    global server_ip, server_port

    #    server_ip = "127.0.0.1"
    
    try:
        server_ip = socket.gethostbyname("WLANSW-1")
    except Exception as error:
        print("cannot find server:", error) 
        exit()

    server_port = 23    # telnet port

    #

    label = tk.Label(root, text="WLAN Switch 1")
    label.grid(row=0, columnspan=10)

    # Create buttons

    button_R = tk.Button(root, text="R", height= 2, width=4, command=send_R_message)
    button_R.grid(row=2, column=2)
    button_r = tk.Button(root, text="r", height= 2, width=4, command=send_r_message)
    button_r.grid(row=2, column=3)   
    button_G = tk.Button(root, text="G", height= 2, width=4, command=send_G_message)
    button_G.grid(row=2, column=4)
    button_g = tk.Button(root, text="g", height= 2, width=4, command=send_g_message)
    button_g.grid(row=2, column=5)
    button_B = tk.Button(root, text="B", height= 2, width=4, command=send_B_message)
    button_B.grid(row=2, column=6)
    button_b = tk.Button(root, text="b", height= 2, width=4, command=send_b_message)
    button_b.grid(row=2, column=7)
    button_W = tk.Button(root, text="W", height= 2, width=4, command=send_W_message)
    button_W.grid(row=2, column=8)
    button_w = tk.Button(root, text="w", height= 2, width=4, command=send_w_message)
    button_w.grid(row=2, column=9)

    space1 = tk.Label(root, text=" ")
    space1.grid(row=3)
    
    button_1 = tk.Button(root, text="1", command=send_1_message)
    button_1.grid(row=4, column=4)

    button_0 = tk.Button(root, text="0", command=send_0_message)
    button_0.grid(row=4, column=5)

    button_V = tk.Button(root, text="V", command=send_V_message)
    button_V.grid(row=4, column=6)

    button_v = tk.Button(root, text="v", command=send_v_message)
    button_v.grid(row=4, column=7)

    button_Q = tk.Button(root, text="?", command=send_Q_message)
    button_Q.grid(row=4, column=10)

    space2 = tk.Label(root, text=" ")
    space2.grid(row=5)

    # Create label for output

    output_label = tk.Label(root, text="def")

    root.mainloop()

if __name__ == "__main__":
    main()

