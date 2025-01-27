#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>            // struct addrinfo
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_log.h"

#define HOST_IP_ADDR "192.168.0.139"
#define PORT 12345
#define MAXLINE 1024

static const char *TAG = "example";

void tcp_client(float *temp, int *hum, float *press) {
    int sockfd;
    struct sockaddr_in servaddr;
    char recvline[MAXLINE + 1], sendline[MAXLINE];
 
    // Predefined values to be sent to the server
    float tcp_temperature = *temp;      // Temperature
    float tcp_pressure = *press; // Pressure
    int tcp_humidity = *hum; // Humidity

    char host_ip[] = HOST_IP_ADDR;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    
    inet_pton(AF_INET, host_ip, &servaddr.sin_addr);

    sockfd =  socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sockfd < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        exit(1);
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

    int err = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        exit(1);
    }
    ESP_LOGI(TAG, "Successfully connected");

    // Format and send the predefined values
    snprintf(sendline, sizeof(sendline), "%.2f %d %.1f", tcp_temperature, tcp_humidity, tcp_pressure);

    int error = send(sockfd, sendline, strlen(sendline), 0);
    if (error < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        exit(1);
    }

    printf("Sent to server: Temperature=%.2f, Humidity=%d, Pressure=%.1f\n", tcp_temperature, tcp_humidity, tcp_pressure);

    int len = recv(sockfd, recvline, MAXLINE, 0);
    // Error occurred during receiving
    if (len < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
        exit(1);
    }
    // Data received
    else {
        recvline[len] = 0; // Null-terminate whatever we received and treat like a string
        ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
        ESP_LOGI(TAG, "%s", recvline);
    }

    ESP_LOGI(TAG, "Shutting down socket and restarting...");
    shutdown(sockfd, 0);
    close(sockfd);
}
