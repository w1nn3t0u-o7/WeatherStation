#pragma once

#include <iostream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <string>

#include "esp_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wifi.hpp"

namespace MZDK {
    class SNTP {  
    private:
        static std::chrono::_V2::system_clock::time_point m_last_update;
        static bool m_running;

        static void m_callbackOnNtpUpdate(timeval *tv);

    public:
        SNTP() = default;
        ~SNTP() { esp_sntp_stop(); };

        static esp_err_t init();

        static bool setUpdateInterval(uint32_t ms, bool immediate = false);

        [[nodiscard]] bool sntpState() {return m_running;}

        [[nodiscard]] static const auto timePointNow();

        [[nodiscard]] static const auto timeSinceLastUpdate();

        [[nodiscard]] static const char *timeNowAscii();

        [[nodiscard]] static std::chrono::seconds epochSeconds();
    };
} 