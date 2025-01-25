#pragma once

#include <iostream>
#include <cstring>
#include <mutex>

#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "nvs_flash.h"

namespace MZDK {
    class WIFI {
    public:
        enum class state
        {
            NOT_INITIALIZED,
            INITIALIZED,
            READY_TO_CONNECT,
            CONNECTING,
            WAITING_FOR_IP,
            CONNECTED,
            DISCONNECTED,
            ERROR
        };

    private:
        static int m_init();
        static wifi_init_config_t m_wifi_init_config;
        static wifi_config_t m_wifi_config;

        static void m_wifiEventHandler(void *arg, esp_event_base_t event_base,
                                       int32_t event_id, void *event_data);
        static void m_ipEventHandler(void *arg, esp_event_base_t event_base,
                                     int32_t event_id, void *event_data);

        static state m_state;
        static int m_getMac();
        static char m_mac_addr_cstr[13];
        static std::mutex m_mutx;


    public:
        WIFI();

        void setCredentials(const char *ssid, const char *password);
        int init();
        int begin();

        constexpr static const state &getState() {return m_state;}
        constexpr static const char *getMac() {return m_mac_addr_cstr;}
    }; 
}