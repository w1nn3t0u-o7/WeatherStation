#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>


#define PORT 12345

#define MAXLINE 1024
#define LISTENQ 10

static const char *TAG = "example";

static void tcp_server_task(void *pvParameters) {
    int listenfd, connfd, sockfd, maxfd, nready, i;
    int client[FD_SETSIZE];  // Array to store connected client sockets
    fd_set rset, allset;
    char buff[MAXLINE], str[INET_ADDRSTRLEN + 1];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    float temp = 0.0;       // The temperature value to send
    float pressure = 0.0;  // The pressure value to send
    int humidity = 0;  // The humidity value to send

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);  // Port number

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ESP_LOGI(TAG, "Socket created");

    int err = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", AF_INET);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listenfd, LISTENQ);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    maxfd = listenfd;  // Initialize maxfd
    for (i = 0; i < FD_SETSIZE; i++) client[i] = -1;  // Initialize client array

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    ESP_LOGI(TAG, "Socket listening");

    while (true) {
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        if (nready < 0) {
            fprintf(stderr, "select error: %s\n", strerror(errno));
            exit(1);
        }

        // Handle new connections
        if (FD_ISSET(listenfd, &rset)) {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

            if (connfd < 0) {
                fprintf(stderr, "accept error: %s\n", strerror(errno));
                continue;
            }

            bzero(str, sizeof(str));
            inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str));
            printf("New connection from %s\n", str);

            // Send the current sensor data immediately after connection
            snprintf(buff, sizeof(buff), "Temperature: %.2f°C\r\nHumidity: %d%%\r\nPressure: %.1f hPa\r\n",
                     temp, humidity, pressure);
            if (write(connfd, buff, strlen(buff)) < 0) {
                fprintf(stderr, "write error: %s\n", strerror(errno));
                close(connfd);
                continue;
            }

            // Add the new client to the client array
            for (i = 0; i < FD_SETSIZE; i++) {
                if (client[i] < 0) {
                    client[i] = connfd;
                    break;
                }
            }

            if (i == FD_SETSIZE) {
                fprintf(stderr, "Too many clients\n");
                close(connfd);
                continue;
            }

            FD_SET(connfd, &allset);
            if (connfd > maxfd) maxfd = connfd;
            if (--nready <= 0) continue;
        }

        // Handle data from clients
        for (i = 0; i < FD_SETSIZE; i++) {
            if ((sockfd = client[i]) < 0) continue;

            if (FD_ISSET(sockfd, &rset)) {
                int n = read(sockfd, buff, MAXLINE);

                if (n == 0) {
                    // Client disconnected
                    printf("Client disconnected\n");
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                } else {
                    buff[n] = 0;

                    // Parse the received data (expected format: temp humidity pressure)
                    float new_temp, new_pressure;
                    int new_humidity;
                    if (sscanf(buff, "%f %d %f", &new_temp, &new_humidity, &new_pressure) == 3) {
                        // Update the shared sensor values
                        temp = new_temp;
                        humidity = new_humidity;
                        pressure = new_pressure;
                        printf("Updated values: Temp=%.2f, Humidity=%d, Pressure=%.1f\n",
                               temp, humidity, pressure);

                        // Broadcast updated values to all clients
                        snprintf(buff, sizeof(buff), "Temperature: %.2f°C\r\nHumidity: %d%%\r\nPressure: %.1f hPa\r\n",
                                 temp, humidity, pressure);
                        for (int j = 0; j < FD_SETSIZE; j++) {
                            if (client[j] != -1) {
                                if (write(client[j], buff, strlen(buff)) < 0) {
                                    fprintf(stderr, "write error to client: %s\n", strerror(errno));
                                    close(client[j]);
                                    FD_CLR(client[j], &allset);
                                    client[j] = -1;
                                }
                            }
                        }
                    }
                }
                if (--nready <= 0) break;
            }
        }
    }

CLEAN_UP:
    close(listenfd);
    vTaskDelete(NULL);
}

    
