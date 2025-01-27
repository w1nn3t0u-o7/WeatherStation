#pragma once

#include "spi.hpp"
#include "i2c.hpp"
#include "bme280.hpp"
#include "wifi.hpp"
#include "sntp.hpp"
#include "tcp_client.hpp"

namespace MZDK {
    void connectToWifi(WIFI *wifi, SNTP *sntp) {
        MZDK::WIFI::state wifi_state = wifi->getState();
        bool sntp_initialized = sntp->sntpState();

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
            if (!sntp_initialized) {
                sntp->init();
            }
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