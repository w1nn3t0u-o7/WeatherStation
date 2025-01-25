#include "main.hpp"

extern "C" void app_main(void) {
    MZDK::WIFI wifi;
    wifi.setCredentials("MidgetSpinner", "MiKi0987");
    wifi.init();

    while(true) {
            MZDK::connectToWifi(&wifi);
            vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
