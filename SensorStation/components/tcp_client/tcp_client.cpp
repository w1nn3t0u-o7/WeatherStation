/**
 * @file tcp_client.cpp
 * @brief TCP client for communicating with a remote server and sending BME sensor data.
 *
 * This file contains the implementation of the function responsible for sending BME sensor data
 * (temperature, humidity, and pressure) to a remote server and receiving a response.
 */

#include "tcp_client.hpp"

static const char *TAG = "TCP_CLIENT";

/**
 * @brief Sends BME sensor data to the server and receives a response.
 *
 * This function creates a socket, connects to the remote server at the specified IP address and port,
 * sends the temperature, humidity, and pressure values to the server, and then waits for a response.
 * The response is printed to the log. If any errors occur during the process, they are logged.
 *
 * @param[in] temp Pointer to a float containing the temperature data to be sent.
 * @param[in] hum Pointer to an integer containing the humidity data to be sent.
 * @param[in] press Pointer to a float containing the pressure data to be sent.
 */
void tcpClientBmeData(float *temp, int *hum, float *press) {
    int sock_fd;
    struct sockaddr_in serv_addr;
    char recv_line[MAXLINE + 1], send_line[MAXLINE];

    // Predefined values to be sent to the server
    float bme_temperature = *temp;      ///< Temperature value
    float bme_pressure = *press; ///< Pressure value
    int bme_humidity = *hum; ///< Humidity value

    char host_ip[] = HOST_IP_ADDR;

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    inet_pton(AF_INET, host_ip, &serv_addr.sin_addr);

    // Create socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        exit(1);
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

    // Connect to server
    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        exit(1);
    }
    ESP_LOGI(TAG, "Successfully connected");

    // Format and send the predefined values
    snprintf(send_line, sizeof(send_line), "%.2f %d %.1f", bme_temperature, bme_humidity, bme_pressure);

    // Send data to the server
    if (send(sock_fd, send_line, strlen(send_line), 0) < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        exit(1);
    }

    ESP_LOGI(TAG, "Sent to server: Temperature=%.2f, Humidity=%d, Pressure=%.1f\n", bme_temperature, bme_humidity, bme_pressure);

    // Receive response from server
    int len = recv(sock_fd, recv_line, MAXLINE, 0);
    if (len < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
        exit(1);
    }
    else {
        recv_line[len] = 0; // Null-terminate the received data
        ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
        ESP_LOGI(TAG, "%s", recv_line);
    }

    ESP_LOGI(TAG, "Shutting down socket and restarting...");
    shutdown(sock_fd, 0);
    close(sock_fd);
}
