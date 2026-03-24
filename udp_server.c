#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3000     #the default port number the server will run on

struct pdu {             //PDU is the protocol data unit
    char type;
    char data[100];   
};

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server, client;
    socklen_t client_len;               //stores size of client address structure
    struct pdu packet;               // to receive request from client

    int port = PORT;
    if (argc == 2) port = atoi(argv[1]);    //if user enters a port number it will override the default port number

    sock = socket(AF_INET, SOCK_DGRAM, 0);   // create udp socket and AF_INET is IPv4

    server.sin_family = AF_INET;       //mspecify IPv4 addresing
    server.sin_addr.s_addr = INADDR_ANY;   //accept connections from any IP address
    server.sin_port = htons(port);       //convert the port to network byte order

    bind(sock, (struct sockaddr *)&server, sizeof(server));  //binding the socket to IP and port so it can receive data

    printf("Server running on port %d...\n", port);

    client_len = sizeof(client);   //client address length

    while (1) {    //this is an infinite loop so that it can ahndle multiple client requests
  
        int n = recvfrom(sock, &packet, sizeof(packet), 0,
                         (struct sockaddr *)&client, &client_len);            //receives the data from client, stores it in packet and gets client address

        if (n <= 0) continue;       //if nothing is received it is gonna wait again

        printf("Client requested file: %s\n", packet.data);

        FILE *fp = fopen(packet.data, "r");   //open requested file in read mode

        struct pdu response;   //pdu is used to send response to client

        if (fp == NULL) {      //if the file doesnt exist
            response.type = 'E';   //e is error
            strcpy(response.data, "File not found");

            sendto(sock, &response, strlen(response.data) + 1 + 1, 0,
                   (struct sockaddr *)&client, client_len);            
        } else {
            while (1) {    //loop to send file in chunks
                int bytes = fread(response.data, 1, 100, fp);   //read up to 100 bytes from the file

                if (bytes < 100) {
                    response.type = 'F';  //if less than 100 bytes it will give the last 100. F means final packet
                    sendto(sock, &response, bytes + 1, 0,
                           (struct sockaddr *)&client, client_len);
                    break;
                } else {
                    response.type = 'D';   //sends the normal d means data packet
                    sendto(sock, &response, bytes + 1, 0,
                           (struct sockaddr *)&client, client_len);
                }
            }
            fclose(fp);
        }
    }

    return 0;
}
