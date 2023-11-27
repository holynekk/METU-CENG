# Mert Kaan YILMAZ - 2381093

import socket
import threading
import pickle
import sys
import time

# Util function to cost of path between nodes
def print_distance_vector(distance_vector):
    for port in distance_vector:
        print(f"{PORT} -{port} | {distance_vector[port]}")

# Server connection thread's util function
def server_side(cost_for_ports):
    global client_send_mode_on, is_server_on
    # Create a socket to listen sent messages
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Bind it to host and then start to listen
    server_socket.bind((HOST, PORT))
    server_socket.listen()
    while True:
        try:
            # Wait for a connection
            conn = server_socket.accept()[0]
            # After connection, wait for receiving data and convert binary message to list
            msg = pickle.loads(conn.recv(1024))
            # Try to update the path costs
            cost = cost_for_ports[msg['sent_from']]
            # Loop through all ports to update costs
            for port in ALL_PORTS:
                joined_cost = cost + msg[port]
                if joined_cost < cost_for_ports[port]:
                    # Update the cost
                    cost_for_ports[port] = joined_cost
                    # Set this flag as True and let client to send a new message
                    client_send_mode_on = True
            # Close the connection after the job is done
            conn.close()
        # Timeout occurs after 10 secs if there is no update
        # I could not set this timeout as 5 secs as mentioned in the pdf. Whenever I decrease
        # the timout number, it cannot connet to server for some reason. ([ALREADY ON USE] issue)
        except socket.timeout:
            # Print the result for a specific port (node)
            print_distance_vector(cost_for_ports)
            # To kill the client thread, set this flag as False
            is_server_on = False
            # Kill the server thread
            sys.exit()

# Client connection thread's util function
def client_side(cost_for_ports, connectionPorts):
    global client_send_mode_on
    # Try to send messages from client as long as server is on
    while is_server_on:
        # Check if port is not being used right now
        if client_send_mode_on:
            # To block the port set this flag as False
            client_send_mode_on = False
            for port in connectionPorts:
                is_connected = False
                while not is_connected:
                    try:
                        # Create a socket for client and send the message through it
                        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                        # Connect it to one of the neighbour nodes
                        client_socket.connect((HOST, port))
                        is_connected = True
                    except OSError:
                        time.sleep(1)
                d_vector_w_cient_inf = dict(cost_for_ports)
                # Add the information of "who (which port) sent this message"
                d_vector_w_cient_inf["sent_from"] = PORT
                # Send the message (distance vector) to server's relevant port
                client_socket.send(pickle.dumps(d_vector_w_cient_inf))
                # Close the socket connection after sending the message
                client_socket.close()
    # Kill the client thread
    sys.exit()

if __name__ == "__main__":
    # Server constants | Get the port number from the commandline args
    HOST = "127.0.0.1"
    PORT = int(sys.argv[1])
    # Flags to block or run thread loops
    is_server_on = client_send_mode_on = True
    # Variables to use in Bellman-Ford routing algorithm
    connectionPorts = list()
    cost_for_ports = dict()
    # Open file and parse it
    # -> Read {current_port}.costs
    # -> {number_of_nodes}
    # -> {neighbour_nodes_port} {distance_between(current_port, neighbour_nodes_port)}
    # -> ...
    f = open(f"{PORT}.costs", "r")
    # Read the entire text in .costs file
    text = f.read().split("\n")
    # Assign number of nodes
    num_of_nodes = int(text[0])
    ALL_PORTS = [3000+j for j in range(num_of_nodes)]
    # Set all costs infinite initially, except the current node's distance
    for port in ALL_PORTS:
        cost_for_ports[port] = float('inf')
    cost_for_ports[PORT] = 0
    # Set the costs of reaching to instant neighbour
    for row_ind in range(1, len(text)):
        if text[row_ind] != "":
            d_port, cost = [int(t) for t in text[row_ind].split(" ")]
            cost_for_ports[d_port] = cost
            connectionPorts.append(d_port)
    # Set timeout variable for sockets
    socket.setdefaulttimeout(10)
    # Creating threads to implement communication in non-blocking type
    t1 = threading.Thread(target = client_side, args = [cost_for_ports, connectionPorts])
    t2 = threading.Thread(target = server_side, args = [cost_for_ports])
    # Start threads asynchronously
    t1.start()
    t2.start()
