idf_component_register(SRCS wifi.cpp
                    INCLUDE_DIRS .
                    REQUIRES esp_wifi nvs_flash esp_event esp_netif lwip)