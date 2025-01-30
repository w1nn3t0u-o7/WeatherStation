/**
 * @file tcp_client.h
 * @brief TCP client definitions for ESP platform.
 *
 * This file contains macro definitions and function declarations for a TCP client 
 * that communicates with a remote server and retrieves data.
 */

#include "lwip/netdb.h"            
#include "esp_log.h"

/// @brief IP address of the remote host to connect to.
#define HOST_IP_ADDR "192.168.0.139"

/// @brief Port number used by the TCP client to communicate with the server.
#define PORT 12345

/// @brief Maximum size of the buffer for receiving data.
#define MAXLINE 1024

/**
 * @brief TCP client function for retrieving BME sensor data.
 *
 * @param[out] temp Pointer to a float where the temperature value will be stored.
 * @param[out] hum Pointer to an integer where the humidity value will be stored.
 * @param[out] press Pointer to a float where the pressure value will be stored.
 */
void tcpClientBmeData(float *temp, int *hum, float *press);


