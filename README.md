C-Chat-Application

Description
A simple chat application written in C, featuring a server running on CentOS and clients running on Fedora. This project demonstrates basic socket programming and inter-process communication using forked processes to handle multiple clients.

Getting Started
Prerequisites
Ensure you have the necessary tools and libraries installed on your CentOS and Fedora systems:

GCC (GNU Compiler Collection)
Basic understanding of socket programming in C
Proper DHCP and DNS configurations for network communication

Instructions
CentOS (Server)
Create the server file:
vi server.c

Copy the server code into server.c:

Compile the server code:
gcc -o server server.c

Run the server:
./server 6000

Fedora (Client)
Create the client file:
vi client.c

Copy the client code into client.c:

Compile the client code twice to create two clients:
gcc -o client1 client.c
gcc -o client2 client.c

Run the clients in two separate terminals:
./client1 <server_ip_address> 6000
./client2 <server_ip_address> 6000

Special Note
Ensure you have the necessary DHCP and DNS configurations to allow the server and clients to communicate over the network.

Usage
Server: ./server 6000
Client: ./client <server_ip_address> 6000 6001 (6000 is the server port and 6001 is the client port number)