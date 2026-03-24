#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3000

struct pdu {
    char type;
    char data[100];
};

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;           //stores server address
    struct pdu packet;                   // for sending and receiving data

    char *server_ip = "127.0.0.1";         // default server IP which is localhost
    int port = PORT;

    if (argc >= 2) server_ip = argv[1];
    if (argc == 3) port = atoi(argv[2]);


    sock = socket(AF_INET, SOCK_DGRAM, 0);   //create UDP socket

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server.sin_addr);  //converts IP string to binary form

    connect(sock, (struct sockaddr *)&server, sizeof(server));  //associates socket with server address and lets read and write happen 

    while (1) {   //infinit loop to allow multiple downloads
        printf("Enter filename (or type quit): ");
        scanf("%s", packet.data);                 //read file form user

        if (strcmp(packet.data, "quit") == 0) break;  //exit if user typed quit

        packet.type = 'C';  //c means filenae request

        write(sock, &packet, strlen(packet.data) + 1 + 1);   //send filename to server

        FILE *fp = fopen("output.txt", "w");    //create file named output.txt to store the doanloaded data

        while (1) {  //infinit loop to receive file data
            int n = read(sock, &packet, sizeof(packet));

            if (n <= 0) break;

            if (packet.type == 'E') {   //if error 
                printf("Error: %s\n", packet.data);
                break;
            }

            fwrite(packet.data, 1, n - 1, fp);   //write received data to fil. n-1 removes type byte

            if (packet.type == 'F') {   //if final packet received
                printf("Download complete\n");
                break;
            }
        }

        fclose(fp);
    }

    close(sock);
    return 0;
}
