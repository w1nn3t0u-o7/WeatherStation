idf_component_register(SRCS sntp.cpp
                    INCLUDE_DIRS . 
                    REQUIRES wifi esp_wifi nvs_flash esp_event esp_netif lwip)