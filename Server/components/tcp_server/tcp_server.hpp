#include "esp_log.h"

#include <lwip/netdb.h>


#define PORT 12345

#define MAXLINE 1024
#define LISTENQ 10

void tcp_server_task(void *pvParameters);

    
