#include "lwip/netdb.h"            
#include "esp_log.h"

#define HOST_IP_ADDR "192.168.0.139"
#define PORT 12345
#define MAXLINE 1024

void tcpClientBmeData(float *temp, int *hum, float *press, const char *time_now);


