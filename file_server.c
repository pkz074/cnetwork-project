#include <stdio.h>       // fopen, fread, fclose, printf, perror
#include <stdlib.h>      // exit()
#include <string.h>      // memset()
#include <unistd.h>      // close()
#include <sys/socket.h>  // socket, bind, listen, accept, send, recv
#include <netinet/in.h>  // sockaddr_in, INADDR_ANY, htons, htonl

#define PORT  3000
#define CHUNK 99

int main() {
    int sd, new_sd;
    struct sockaddr_in server, client;
    int client_len = sizeof(client);
    int opt = 1;

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket"); exit(1);
    }
    // allow the port to be reused immediately after the server restarts
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server, 0, sizeof(server));
    server.sin_family      = AF_INET;
    server.sin_port        = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind
    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind"); exit(1);
    }
    //listen
    listen(sd, 5);
    printf("server listening on port %d\n", PORT);

    while (1) {
        new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
        if (new_sd < 0) {
            perror("accept"); continue;
        }
        printf("connected\n");
        //filename from client
        char filename[100];
        int n = recv(new_sd, filename, sizeof(filename) - 1, 0);
        if (n <= 0) {
            close(new_sd); continue;
        }
        filename[n] = '\0';
        printf("requested: %s\n", filename);

        FILE *fp = fopen(filename, "rb");

        //error
        if (fp == NULL) {
            char errmsg[100];
            int len = snprintf(errmsg + 1, sizeof(errmsg) - 1,
                               "file '%s' not found", filename);
            errmsg[0] = 'E';
            send(new_sd, errmsg, len + 1, 0);
            printf("file not found: %s\n", filename);
        //received file
        } else {
            char buf[CHUNK + 1];
            buf[0] = 'D';

            while ((n = fread(buf + 1, 1, CHUNK, fp)) > 0) {
                send(new_sd, buf, n + 1, 0);
            }

            fclose(fp);
            printf("file sent: %s\n", filename);
        }

        close(new_sd);
    }
}
