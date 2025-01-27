/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
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
    int temp = 17;       // The temperature value to send
    float humidity = 31.2;  // The humidity value to send
    float pressure = 40.5;  // The pressure value to send

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
        rset = allset;  // Copy the master set
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);  // Wait for an event

        if (nready < 0) {
            fprintf(stderr, "select error: %s\n", strerror(errno));
            exit(1);
        }

        // Check for new connection
        if (FD_ISSET(listenfd, &rset)) {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
            if (connfd < 0) {
                ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
                break;
            }

            // Convert ip address to string
            bzero(str, sizeof(str));
            inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str));
            ESP_LOGI(TAG, "Socket accepted ip address: %s", str);

            // Send the temperature, humidity, and pressure to the new client immediately
            snprintf(buff, sizeof(buff), "Temperature: %dÂ°C\r\nHumidity: %.1f%%\r\nPressure: %.1f hPa\r\n", temp, humidity, pressure);
            if (write(connfd, buff, strlen(buff)) < 0) {
                fprintf(stderr, "write error: %s\n", strerror(errno));
                close(connfd);
                continue;
            }

            for (i = 0; i < FD_SETSIZE; i++) {
                if (client[i] < 0) {
                    client[i] = connfd;  // Save the socket
                    break;
                }
            }

            if (i == FD_SETSIZE) {
                fprintf(stderr, "Too many clients\n");
                close(connfd);
                continue;
            }

            FD_SET(connfd, &allset);  // Add the new connection to the set
            if (connfd > maxfd) maxfd = connfd;  // Update maxfd
            if (--nready <= 0) continue;  // No more descriptors ready
        }

        // Check all clients for data
        for (i = 0; i < FD_SETSIZE; i++) {
            if ((sockfd = client[i]) < 0) continue;

            if (FD_ISSET(sockfd, &rset)) {
                if (read(sockfd, buff, MAXLINE) == 0) {
                    // Client disconnected
                    printf("Client disconnected\n");
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                }
                if (--nready <= 0) break;  // No more descriptors ready
            }
        }
        // shutdown(connfd, 0);
        // close(connfd);
    }

CLEAN_UP:
    close(listenfd);
    vTaskDelete(NULL);
}

    
