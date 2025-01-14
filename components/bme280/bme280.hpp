#pragma once

#include <memory>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "com_protocols.hpp"

namespace MZDK {
    class BME280 {
        // Registers
        constexpr static uint8_t HUM_LSB = 0xFE;
        constexpr static uint8_t HUM_MSB = 0xFD;
        constexpr static uint8_t TEMP_XLSB = 0xFC;
        constexpr static uint8_t TEMP_LSB = 0xFB;
        constexpr static uint8_t TEMP_MSB = 0xFA;
        constexpr static uint8_t PRESS_XLSB = 0xF9;
        constexpr static uint8_t PRESS_LSB = 0xF8;
        constexpr static uint8_t PRESS_MSB = 0xF7;
        constexpr static uint8_t CONFIG = 0xF5;
        constexpr static uint8_t CTRL_MEAS = 0xF4;
        constexpr static uint8_t STATUS = 0xF3;
        constexpr static uint8_t CTRL_HUM = 0xF2;
        constexpr static uint8_t RESET = 0xE0;
        constexpr static uint8_t ID = 0xD0;

        // Settings
        constexpr static uint8_t pressure_sensor_disable = 0x00 << 2;
        constexpr static uint8_t pressure_oversampling_X1 = 0x01 << 2;
        constexpr static uint8_t pressure_oversampling_X2 = 0x02 << 2;
        constexpr static uint8_t pressure_oversampling_X4 = 0x03 << 2;
        constexpr static uint8_t pressure_oversampling_X8 = 0x04 << 2;
        constexpr static uint8_t pressure_oversampling_X16 = 0x05 << 2;
        constexpr static uint8_t temperature_sensor_disable = 0x00 << 5;
        constexpr static uint8_t temperature_oversampling_X1 = 0x01 << 5;
        constexpr static uint8_t temperature_oversampling_X2 = 0x02 << 5;
        constexpr static uint8_t temperature_oversampling_X4 = 0x03 << 5;
        constexpr static uint8_t temperature_oversampling_X8 = 0x04 << 5;
        constexpr static uint8_t temperature_oversampling_X16 = 0x05 << 5;
        constexpr static uint8_t sensor_sleep_mode = 0x00;
        constexpr static uint8_t sensor_forced_mode = 0x01;
        constexpr static uint8_t sensor_normal_mode = 0x03;

        constexpr static uint8_t config_standby_0_5ms = 0x00 << 5;
        constexpr static uint8_t config_standby_62_5ms = 0x01 << 5;
        constexpr static uint8_t config_standby_125ms = 0x02 << 5;
        constexpr static uint8_t config_standby_250ms = 0x03 << 5;
        constexpr static uint8_t config_standby_500ms = 0x04 << 5;
        constexpr static uint8_t config_standby_1000ms = 0x05 << 5;
        constexpr static uint8_t config_standby_10ms = 0x06 << 5;
        constexpr static uint8_t config_standby_20ms = 0x07 << 5;
        constexpr static uint8_t config_silter_off = 0x00 << 2;
        constexpr static uint8_t config_silter_2 = 0x01 << 2;
        constexpr static uint8_t config_filter_4 = 0x02 << 2;
        constexpr static uint8_t config_filter_8 = 0x03 << 2;
        constexpr static uint8_t config_filter_16 = 0x04 << 2;

        constexpr static uint8_t humidity_sensor_disable = 0x00;
        constexpr static uint8_t humidity_oversampling_X1 = 0x01;
        constexpr static uint8_t humidity_oversampling_X2 = 0x02;
        constexpr static uint8_t humidity_oversampling_X4 = 0x03;
        constexpr static uint8_t humidity_oversampling_X8 = 0x04;
        constexpr static uint8_t humidity_oversampling_X16 = 0x05;

    private:
        ComProtocol *m_com_protocol;

        struct m_SensorRawData
        {
            long m_temperature = 0;
            unsigned long m_humididty = 0;
            unsigned long m_pressure = 0;
        };

        uint8_t m_humidity_oversampling_value = humidity_oversampling_X1;    // Default to 1X over sampling
        uint8_t m_temperature_oversampling_value = temperature_oversampling_X1; // Default to 1X over sampling
        uint8_t m_pressure_oversampling_value = pressure_oversampling_X1;    // Default to 1X over sampling
        
        uint8_t m_sensor_mode_value = sensor_forced_mode;              // Default to forced mode

        // Calibration Data
        unsigned short  dig_t1 = 0;
        signed short    dig_t2 = 0;
        signed short    dig_t3 = 0;
        signed long     t_fine = 0;
        unsigned short  dig_p1 = 0;
        signed short    dig_p2 = 0;
        signed short    dig_p3 = 0;
        signed short    dig_p4 = 0;
        signed short    dig_p5 = 0;
        signed short    dig_p6 = 0;
        signed short    dig_p7 = 0;
        signed short    dig_p8 = 0;
        signed short    dig_p9 = 0;
        uint8_t         dig_h1 = 0;
        signed short    dig_h2 = 0;
        uint8_t         dig_h3 = 0;
        signed short    dig_h4 = 0;
        signed short    dig_h5 = 0;
        signed char     dig_h6 = 0;

        int getStatus();
        int getCalibrateData();
        int getSensorData(m_SensorRawData *result_raw);
        float compensateTemp(const signed long adc_T);
        float compensatePressure(const unsigned long adc_P);
        int compensateHumidity(const unsigned long adc_H);
        
    public:
        BME280(ComProtocol *com_protocol, const uint8_t humidity_oversampling = humidity_oversampling_X1,
                           const uint8_t temperature_oversampling = temperature_oversampling_X1,
                           const uint8_t pressure_oversampling = pressure_oversampling_X1,
                           const uint8_t sensor_mode = sensor_forced_mode);

        struct BME280ResultData {
            float temperature = 0.0;
            int humididty = 0;
            float pressure = 0.0;
        } results;

        int getDeviceID();
        int setConfig(const uint8_t config);
        int setConfigStandbyT(const uint8_t standby);   // config bits 7, 6, 5  page 30
        int setConfigFilter(const uint8_t filter);      // config bits 4, 3, 2
        int setCtrlMeas(const uint8_t ctrl_measure);
        int setTemperatureOversampling(const uint8_t temperature_oversampling);     // ctrl_meas bits 7, 6, 5   page 29
        int setPressureOversampling(const uint8_t pressure_oversampling);    // ctrl_meas bits 4, 3, 2
        int setOversampling(const uint8_t temperature_oversampling, const uint8_t pressure_oversampling);
        int setMode(const uint8_t mode);                                    // ctrl_meas bits 1, 0
        int setCtrlHum(const int humididtyOversampling);                    // ctrl_hum bits 2, 1, 0    page 28
        int getAllResults(BME280ResultData *results);
        int getAllResults(float *temperature, int *humidity, float *pressure);
        float getTemperature();    // Preferable to use GetAllResults()
        float getPressure();       
        int getHumidity();       
        bool statusMeasuringBusy(); // check status (0xF3) bit 3
        bool imUpdateBusy();        // check status (0xF3) bit 0
        int reset();                // write 0xB6 into reset (0xE0)
    };
} 