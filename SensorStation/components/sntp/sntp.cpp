#include "sntp.hpp"

namespace MZDK {
    //Statics
    std::chrono::_V2::system_clock::time_point SNTP::m_last_update {};
    bool SNTP::m_running = false;

    void SNTP::m_callbackOnNtpUpdate(timeval *tv) {
        std::cout << "NTP updated, current time is: " << timeNowAscii() << '\n';
        m_last_update = std::chrono::system_clock::now();
    }

    int SNTP::init() {
        if (!m_running) {
            if(WIFI::getState() != WIFI::state::CONNECTED) {
                std::cout << "Failed to initialise SNTP, Wifi not connected\n";
                return 1;
            }

            setenv("TZ", "GMT-1", 1); // Poland Time (GMT+1)
            tzset();

            esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
            esp_sntp_setservername(0, "time.google.com");
            esp_sntp_setservername(1, "pool.ntp.com");

            sntp_set_time_sync_notification_cb(&m_callbackOnNtpUpdate);
            sntp_set_sync_interval(60 * 60 * 1000); // Update time every hour

            esp_sntp_init();

            std::cout << "SNTP Initialised\n";

            m_running = true;
        }

        if (m_running) {
            return 0;
        }
        return 1;
    }

    bool SNTP::setUpdateInterval(uint32_t ms, const bool immediate) {
        if (m_running) {
            sntp_set_sync_interval(ms);
            if (immediate) {
                sntp_restart();
            }
            return true;
        }
        return false;
    }

    [[nodiscard]] const auto SNTP::timePointNow() {
        return std::chrono::system_clock::now();
    }

    [[nodiscard]] const auto SNTP::timeSinceLastUpdate() {
        return timePointNow() - m_last_update;
    }

    [[nodiscard]] std::chrono::seconds SNTP::epochSeconds() {
        return std::chrono::duration_cast<std::chrono::seconds>(timePointNow().time_since_epoch());
    }

    [[nodiscard]] const char *SNTP::timeNowAscii() {
        const std::time_t timeNow{std::chrono::system_clock::to_time_t(timePointNow())};

        return std::asctime(std::localtime(&timeNow));
    }
}