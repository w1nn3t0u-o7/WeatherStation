#include "tcp_server.hpp"

static const char *TAG = "TCP_SERVER";

void tcp_server_task(void *pvParameters) {
    int listen_fd, conn_fd, sock_fd, max_fd, n_ready, i;
    int client[FD_SETSIZE];  // Array to store connected client sockets
    fd_set rset, allset;
    char buff[MAXLINE], str[INET_ADDRSTRLEN + 1];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    float bme_temperature = 0.0;       // The temperature value to send
    float bme_pressure = 0.0;  // The pressure value to send
    int bme_humidity = 0;  // The humidity value to send

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);  // Port number

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", AF_INET);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listen_fd, LISTENQ);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    max_fd = listen_fd;  // Initialize maxfd
    for (i = 0; i < FD_SETSIZE; i++) client[i] = -1;  // Initialize client array

    FD_ZERO(&allset);
    FD_SET(listen_fd, &allset);

    ESP_LOGI(TAG, "Socket listening");

    while (true) {
        rset = allset;
        n_ready = select(max_fd + 1, &rset, NULL, NULL, NULL);

        if (n_ready < 0) {
            ESP_LOGE(TAG, "select error: %s", strerror(errno));
            exit(1);
        }

        // Handle new connections
        if (FD_ISSET(listen_fd, &rset)) {
            cli_len = sizeof(cli_addr);
            conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len);

            if (conn_fd < 0) {
                ESP_LOGE(TAG, "accept error: %s", strerror(errno));
                continue;
            }

            bzero(str, sizeof(str));
            inet_ntop(AF_INET, &cli_addr.sin_addr, str, sizeof(str));
            ESP_LOGI(TAG, "New connection from %s", str);

            // Send the current sensor data immediately after connection
            snprintf(buff, sizeof(buff), "Temperature: %.2f°C\r\nHumidity: %d%%\r\nPressure: %.1f Pa\r\n",
                     bme_temperature, bme_humidity, bme_pressure);
            if (write(conn_fd, buff, strlen(buff)) < 0) {
                ESP_LOGE(TAG, "write error: %s", strerror(errno));
                close(conn_fd);
                continue;
            }

            // Add the new client to the client array
            for (i = 0; i < FD_SETSIZE; i++) {
                if (client[i] < 0) {
                    client[i] = conn_fd;
                    break;
                }
            }

            if (i == FD_SETSIZE) {
                ESP_LOGE(TAG, "Too many clients");
                close(conn_fd);
                continue;
            }

            FD_SET(conn_fd, &allset);
            if (conn_fd > max_fd) {
                max_fd = conn_fd;
            }

            if (--n_ready <= 0) {
                continue;
            }
        }

        // Handle data from clients
        for (i = 0; i < FD_SETSIZE; i++) {
            if ((sock_fd = client[i]) < 0) {
                continue;
            }

            if (FD_ISSET(sock_fd, &rset)) {
                int n = read(sock_fd, buff, MAXLINE);

                if (n == 0) {
                    // Client disconnected
                    ESP_LOGI(TAG, "Client disconnected");
                    close(sock_fd);
                    FD_CLR(sock_fd, &allset);
                    client[i] = -1;
                } else {
                    buff[n] = 0;

                    // Parse the received data (expected format: temp humidity pressure)
                    float new_temp, new_pressure;
                    int new_humidity;
                    if (sscanf(buff, "%f %d %f", &new_temp, &new_humidity, &new_pressure) == 3) {
                        // Update the shared sensor values
                        bme_temperature = new_temp;
                        bme_humidity = new_humidity;
                        bme_pressure = new_pressure;
                        ESP_LOGI(TAG, "Updated values: Temp=%.2f, Humidity=%d, Pressure=%.1f",
                               bme_temperature, bme_humidity, bme_pressure);

                        // Broadcast updated values to all clients
                        snprintf(buff, sizeof(buff), "Temperature: %.2f°C\r\nHumidity: %d%%\r\nPressure: %.1f Pa\r\n",
                                 bme_temperature, bme_humidity, bme_pressure);

                        for (int j = 0; j < FD_SETSIZE; j++) {
                            if (client[j] != -1) {
                                if (write(client[j], buff, strlen(buff)) < 0) {
                                    ESP_LOGE(TAG, "write error to client: %s", strerror(errno));
                                    close(client[j]);
                                    FD_CLR(client[j], &allset);
                                    client[j] = -1;
                                }
                            }
                        }
                    }
                }
                if (--n_ready <= 0) break;
            }
        }
    }

CLEAN_UP:
    close(listen_fd);
    vTaskDelete(NULL);
}