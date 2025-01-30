#include "tcp_client.hpp"

static const char *TAG = "TCP_CLIENT";

void tcpClientBmeData(float *temp, int *hum, float *press) {
    int sock_fd;
    struct sockaddr_in serv_addr;
    char recv_line[MAXLINE + 1], send_line[MAXLINE];

    // Predefined values to be sent to the server
    float bme_temperature = *temp;      // Temperature
    float bme_pressure = *press; // Pressure
    int bme_humidity = *hum; // Humidity

    char host_ip[] = HOST_IP_ADDR;

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    inet_pton(AF_INET, host_ip, &serv_addr.sin_addr);

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        exit(1);
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        exit(1);
    }
    ESP_LOGI(TAG, "Successfully connected");

    // Format and send the predefined values
    snprintf(send_line, sizeof(send_line), "%.2f %d %.1f", bme_temperature, bme_humidity, bme_pressure);

    if (send(sock_fd, send_line, strlen(send_line), 0) < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        exit(1);
    }

    ESP_LOGI(TAG, "Sent to server: Temperature=%.2f, Humidity=%d, Pressure=%.1f\n", bme_temperature, bme_humidity, bme_pressure);

    int len = recv(sock_fd, recv_line, MAXLINE, 0);
    // Error occurred during receiving
    if (len < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
        exit(1);
    }
    // Data received
    else {
        recv_line[len] = 0; // Null-terminate whatever we received and treat like a string
        ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
        ESP_LOGI(TAG, "%s", recv_line);
    }

    ESP_LOGI(TAG, "Shutting down socket and restarting...");
    shutdown(sock_fd, 0);
    close(sock_fd);
}