import socket
import time


def send_udp_message(message, server_ip, server_port):
    try:
        # Erstelle einen UDP-Socket
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # Sende die Nachricht an den Server
        client_socket.sendto(message.encode(), (server_ip, server_port))

        # Empfange die Antwort vom Server
        response, _ = client_socket.recvfrom(1024)

        # Schließe den Socket
        client_socket.close()

        return response.decode()
    except Exception as e:
        return str(e)

def main():
    # Definiere die Server-IP und den Port

#    server_ip = "127.0.0.1"
#    server_ip = "192.168.178.41"

    try:
        server_ip = socket.gethostbyname("WLANSW-1")
    except Exception as error:
        print("cannot find server:", error) 
        exit()
    
    server_port = 23     # use telnet port
 
    # Definiere die zu sendende Nachricht
#    message = "w 20 aa"
    message = "1"
    
    # Sende die Nachricht und empfange die Antwort
    response = send_udp_message(message, server_ip, server_port)

    # Falls die Antwort "err" ist, versuche es erneut
    tries = 1
    while response == "err":
        print("Antwort:",response)
        if tries > 2:
            break
        else:
            tries = tries+1
        print("Fehler erhalten. Versuche es erneut...")
        time.sleep(1)  # Warte 1 Sekunde
        response = send_udp_message(message, server_ip, server_port)
        print(f"Erhaltene Antwort: {response}")
        
    print(f"result: {response}")

if __name__ == "__main__":
    main()
