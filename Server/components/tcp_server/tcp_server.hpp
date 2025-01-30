/**
 * @file tcp_server.h
 * @brief TCP server implementation for ESP platform.
 *
 * This file contains the definition of a simple TCP server task for handling client connections.
 */

#include "esp_log.h"
#include <lwip/netdb.h>

/// @brief Port number on which the server will listen for incoming connections.
#define PORT 12345

/// @brief Maximum length of a received message buffer.
#define MAXLINE 1024

/// @brief Maximum number of queued connections in the listen backlog.
#define LISTENQ 10

/**
 * @brief TCP server task function.
 *
 * This function initializes a TCP server, binds to the specified port, and listens for incoming connections.
 * It processes client requests in a loop and handles network communication.
 *
 * @param[in] pvParameters Pointer to task parameters (if any).
 */
void tcp_server_task(void *pvParameters);


    
