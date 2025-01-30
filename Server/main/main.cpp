#include "main.hpp"

extern "C" void app_main(void) {
    MZDK::WIFI wifi;
    wifi.setCredentials("MidgetSpinner", "MiKi0987");
    wifi.init();

    while(wifi.getState() != MZDK::WIFI::state::CONNECTED) {
            MZDK::connectToWifi(&wifi);
            vTaskDelay(pdMS_TO_TICKS(100));
    }

    xTaskCreate(tcp_server_task, "tcp_server", 8192, (void*)AF_INET, 5, NULL);
}

