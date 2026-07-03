#include <stdio.h>       // fopen, fread, fclose, printf, perror
#include <stdlib.h>      // exit()
#include <string.h>      // memset()
#include <unistd.h>      // close()
#include <sys/socket.h>  // socket, bind, listen, accept, send, recv
#include <netinet/in.h>  // sockaddr_in, INADDR_ANY, htons, htonl

#define DEFAULT_PORT 3000
#define CHUNK 1024

static int send_all(int socket_fd, const void *data, size_t length) {
    const char *cursor = data;

    while (length > 0) {
        ssize_t sent = send(socket_fd, cursor, length, 0);
        if (sent <= 0) return -1;
        cursor += sent;
        length -= (size_t)sent;
    }
    return 0;
}

int main(int argc, char **argv) {
    int sd, new_sd;
    int port = DEFAULT_PORT;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);
    int opt = 1;

    if (argc > 2) {
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        return 1;
    }
    if (argc == 2) port = atoi(argv[1]);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket"); exit(1);
    }
    // allow the port to be reused immediately after the server restarts
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server, 0, sizeof(server));
    server.sin_family      = AF_INET;
    server.sin_port        = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind
    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind"); exit(1);
    }
    //listen
    listen(sd, 5);
    printf("server listening on port %d\n", port);

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

        // Send a one-byte status before the response body.
        if (fp == NULL) {
            char errmsg[100];
            int len = snprintf(errmsg, sizeof(errmsg),
                               "file '%s' not found", filename);
            char status = 'E';
            send_all(new_sd, &status, 1);
            send_all(new_sd, errmsg, (size_t)len);
            printf("file not found: %s\n", filename);
        } else {
            char buf[CHUNK];
            char status = 'D';
            send_all(new_sd, &status, 1);

            while ((n = (int)fread(buf, 1, sizeof(buf), fp)) > 0) {
                if (send_all(new_sd, buf, (size_t)n) < 0) break;
            }

            fclose(fp);
            printf("file sent: %s\n", filename);
        }

        close(new_sd);
    }
}
