#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wifi.hpp"

namespace MZDK {
    void connectToWifi(WIFI *wifi) {
        MZDK::WIFI::state wifi_state = wifi->getState();

        switch (wifi_state) {
        case MZDK::WIFI::state::READY_TO_CONNECT:
            std::cout << "Wifi Status: READY_TO_CONNECT\n";
            wifi->begin();
            break;
        case MZDK::WIFI::state::DISCONNECTED:
            std::cout << "Wifi Status: DISCONNECTED\n";
            wifi->begin();
            break;
        case MZDK::WIFI::state::CONNECTING:
            std::cout << "Wifi Status: CONNECTING\n";
            break;
        case MZDK::WIFI::state::WAITING_FOR_IP:
            std::cout << "Wifi Status: WAITING_FOR_IP\n";
            break;
        case MZDK::WIFI::state::ERROR:
            std::cout << "Wifi Status: ERROR\n";
            break;
        case MZDK::WIFI::state::CONNECTED:
            std::cout << "Wifi Status: CONNECTED\n";
            break;
        case MZDK::WIFI::state::NOT_INITIALIZED:
            std::cout << "Wifi Status: NOT_INITIALIZED\n";
            break;
        case MZDK::WIFI::state::INITIALIZED:
            std::cout << "Wifi Status: INITIALIZED\n";
            break;
        }
    }
}