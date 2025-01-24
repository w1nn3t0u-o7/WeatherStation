#include "wifi.hpp"

namespace MZDK {
    char WIFI::m_mac_addr_cstr[] {};
    std::mutex WIFI::m_mutx {};
    WIFI::state WIFI::m_state = state::NOT_INITIALIZED;
    wifi_init_config_t WIFI::m_wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t WIFI::m_wifi_config {};

    WIFI::WIFI() {
        if (!m_mac_addr_cstr[0]) {
            if (m_getMac() != 0) {
                esp_restart();
            }
        }

        esp_event_loop_create_default();
        nvs_flash_init();
    }

    void WIFI::m_wifiEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
        if (WIFI_EVENT == event_base) {
            const wifi_event_t event_type{static_cast<wifi_event_t>(event_id)};

            switch (event_type) {
            case WIFI_EVENT_STA_START:
            {
                std::lock_guard<std::mutex> state_guard(m_mutx);
                m_state = state::READY_TO_CONNECT;
                break;
            }
            case WIFI_EVENT_STA_CONNECTED:
            {
                std::lock_guard<std::mutex> state_guard(m_mutx);
                m_state = state::WAITING_FOR_IP;
                break;
            }
            case WIFI_EVENT_STA_DISCONNECTED:
            {
                std::lock_guard<std::mutex> state_guard(m_mutx);
                m_state = state::DISCONNECTED;
                break;
            }
            default:
                break;
            }
        }
    }

    void WIFI::m_ipEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
        if (IP_EVENT == event_base) {
            const ip_event_t event_type{static_cast<ip_event_t>(event_id)};

            switch (event_type) {
            case IP_EVENT_STA_GOT_IP:
            {
                std::lock_guard<std::mutex> state_guard(m_mutx);
                m_state = state::CONNECTED;
                break;
            }
            case IP_EVENT_STA_LOST_IP:
            {
                std::lock_guard<std::mutex> state_guard(m_mutx);
                if (state::DISCONNECTED != m_state) {
                    m_state = state::WAITING_FOR_IP;
                }
                break;
            }
            default:
                break;
            }
        }
    }

    int WIFI::begin() {
        std::lock_guard<std::mutex> connect_guard(m_mutx);

        int status = 0;

        switch (m_state) {
        case state::READY_TO_CONNECT:
        case state::DISCONNECTED:
            status = esp_wifi_connect();
            if (status == 0) {
                m_state = state::CONNECTING;
            }
            break;
        case state::CONNECTING:
        case state::WAITING_FOR_IP:
        case state::CONNECTED:
            break;
        case state::NOT_INITIALIZED:
        case state::INITIALIZED:
        case state::ERROR:
            status = 1;
            break;
        }
        return status;
    }

    int WIFI::m_init() {
        std::lock_guard<std::mutex> mutx_guard(m_mutx);

        int status = 0;

        if (state::NOT_INITIALIZED == m_state) {
            status |= esp_netif_init();
            if (status == 0) {
                const esp_netif_t *const p_netif = esp_netif_create_default_wifi_sta();

                if (!p_netif) {
                    status = 1;
                }
            }

            if (status == 0) {
                status = esp_wifi_init(&m_wifi_init_config);
            }

            if (status == 0) {
                status = esp_event_handler_instance_register(WIFI_EVENT,
                                                             ESP_EVENT_ANY_ID,
                                                             &m_wifiEventHandler,
                                                             nullptr,
                                                             nullptr);
            }

            if (status == 0) {
                status = esp_event_handler_instance_register(IP_EVENT,
                                                             ESP_EVENT_ANY_ID,
                                                             &m_ipEventHandler,
                                                             nullptr,
                                                             nullptr);
            }

            if (status == 0) {
                status = esp_wifi_set_mode(WIFI_MODE_STA);
            }

            if (status == 0) {
                m_wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
                m_wifi_config.sta.pmf_cfg.capable = true;
                m_wifi_config.sta.pmf_cfg.required = false;

                status = esp_wifi_set_config(WIFI_IF_STA, &m_wifi_config);
            }

            if (status == 0) {
                status = esp_wifi_start(); // start Wifi
            }

            if (status == 0) {
                m_state = state::INITIALIZED;
            }
        } else if (state::ERROR == m_state) {
            m_state = state::NOT_INITIALIZED;
        }
        return status;
    }

    void WIFI::setCredentials(const char *ssid, const char *password) {
        memcpy(m_wifi_config.sta.ssid, ssid, std::min(strlen(ssid), sizeof(m_wifi_config.sta.ssid)));

        memcpy(m_wifi_config.sta.password, password, std::min(strlen(password), sizeof(m_wifi_config.sta.password)));
    }

    int WIFI::init() {
        return m_init();
    }

    // Get default MAC from API and convert to ASCII HEX
    int WIFI::m_getMac() {
        uint8_t mac_byte_buffer[6] {};

        const int status = esp_efuse_mac_get_default(mac_byte_buffer);

        if (status == 0) {
            snprintf(m_mac_addr_cstr, sizeof(m_mac_addr_cstr), "%02X%02X%02X%02X%02X%02X",
                     mac_byte_buffer[0],
                     mac_byte_buffer[1],
                     mac_byte_buffer[2],
                     mac_byte_buffer[3],
                     mac_byte_buffer[4],
                     mac_byte_buffer[5]);
        }
        return status;
    }
} 