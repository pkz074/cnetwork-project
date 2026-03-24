#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT  3000
#define CHUNK 100
#define IP_ADDR "127.0.0.1" // this can be overriden by argv[1],
//so when can do this ./file_client 192.168.0.10 <- to override the IP_ADDR


int main(int argc, char **argv) {
    int sd;
    struct sockaddr_in server;
    char filename[100];
    char buf[CHUNK];
    int n;
                    //ipv4, sock_stream this is tcp
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket"); exit(1);
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port   = htons(PORT);

    // this is what line 12 explained
    const char *ip = (argc >= 2) ? argv[1] : IP_ADDR;
    // convert IP -> 127.0.0.1 to binary form (struct needs)
    if (inet_pton(AF_INET, ip, &server.sin_addr) <= 0) {
        perror("inet_pton"); exit(1);
    }
    //connect to server
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect"); exit(1);
    }

    printf("enter filename to download: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';


    // include the null terminator
    send(sd, filename, strlen(filename) + 1, 0);
    // the first byte is always a flag: 'D' for data, 'E' for error
    n = recv(sd, buf, sizeof(buf), 0);
    if (n <= 0) {
        printf("no response\n");
        close(sd); return 1;
    }

    char type = buf[0]; //flag byte

    //error
    if (type == 'E') {
        printf("server error: %.*s\n", n - 1, buf + 1);
    // works
    } else if (type == 'D') {
        FILE *fp = fopen(filename, "wb"); //binary
        if (!fp) { perror("fopen"); close(sd); return 1; }


        fwrite(buf + 1, 1, n - 1, fp); // first chunk, skip flag byte

        while ((n = recv(sd, buf, sizeof(buf), 0)) > 0)
            fwrite(buf, 1, n, fp); // remaining chunks

        fclose(fp);
        printf("downloaded '%s' successfully\n", filename);
    }

    close(sd);
    return 0;
}
