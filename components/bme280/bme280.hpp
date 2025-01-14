/**
 * @file bme280.hpp
 * @brief Driver for the BME280 sensor.
 * 
 * Provides an interface for configuring and reading data from the BME280 sensor,
 * including temperature, humidity, and pressure measurements.
 */

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "com_protocols.hpp"

namespace MZDK {
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
    /**
     * @class BME280
     * @brief Interface for the BME280 sensor.
     * 
     * This class supports reading and configuring the BME280 sensor, including
     * calibration, oversampling settings, and operating modes.
     */
    class BME280 {
    private:
        ComProtocol *m_com_protocol; ///< Communication protocol (e.g., I2C, SPI).

        /**
         * @struct m_SensorRawData
         * @brief Raw data structure for sensor readings.
         */
        struct m_SensorRawData
        {
            long m_temperature = 0;
            unsigned long m_humididty = 0;
            unsigned long m_pressure = 0;
        };

        uint8_t m_humidity_oversampling_value; ///< Humidity oversampling setting.
        uint8_t m_temperature_oversampling_value; ///< Temperature oversampling setting.
        uint8_t m_pressure_oversampling_value; ///< Pressure oversampling setting.
        
        uint8_t m_sensor_mode_value = sensor_forced_mode; ///< Operating mode.

        // Calibration data for the sensor.
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

        /**
         * @brief Get the sensor status.
         * @return Status of the sensor.
         */
        int getStatus();

        /**
         * @brief Retrieve calibration data from the sensor.
         * @return Status of the operation.
         */
        int getCalibrateData();

        /**
         * @brief Get raw data from the sensor.
         * @param result_raw Pointer to structure to store raw data.
         * @return Status of the operation.
         */
        int getSensorData(m_SensorRawData *result_raw);

        /**
         * @brief Compensate temperature using raw ADC data.
         * @param adc_T Raw ADC temperature value.
         * @return Compensated temperature in degrees Celsius.
         */
        float compensateTemp(const signed long adc_T);

        /**
         * @brief Compensate pressure using raw ADC data.
         * @param adc_P Raw ADC pressure value.
         * @return Compensated pressure in hPa.
         */
        float compensatePressure(const unsigned long adc_P);

        /**
         * @brief Compensate humidity using raw ADC data.
         * @param adc_H Raw ADC humidity value.
         * @return Compensated humidity in percentage.
         */
        int compensateHumidity(const unsigned long adc_H);
        
    public:
        /**
         * @brief Constructs a BME280 object.
         * @param com_protocol Pointer to the communication protocol instance.
         * @param humidity_oversampling Humidity oversampling setting (default: 1X).
         * @param temperature_oversampling Temperature oversampling setting (default: 1X).
         * @param pressure_oversampling Pressure oversampling setting (default: 1X).
         * @param sensor_mode Sensor operating mode (default: forced mode).
         */
        BME280(ComProtocol *com_protocol, const uint8_t humidity_oversampling = humidity_oversampling_X1,
                           const uint8_t temperature_oversampling = temperature_oversampling_X1,
                           const uint8_t pressure_oversampling = pressure_oversampling_X1,
                           const uint8_t sensor_mode = sensor_forced_mode);

        /**
         * @struct BME280ResultData
         * @brief Processed data structure for sensor readings.
         */
        struct BME280ResultData {
            float temperature = 0.0;
            int humididty = 0;
            float pressure = 0.0;
        } results;

        /**
         * @brief Retrieve the device ID.
         * @return Device ID.
         */
        int getDeviceID();
        
        /**
         * @brief Sets the sensor configuration.
         * @param config Configuration byte to set.
         * @return Status of the operation.
         */
        int setConfig(const uint8_t config);

        /**
         * @brief Sets the standby time in the configuration register.
         * @param standby Standby time setting.
         * @return Status of the operation.
         */
        int setConfigStandbyT(const uint8_t standby);

        /**
         * @brief Sets the filter coefficient in the configuration register.
         * @param filter Filter coefficient setting.
         * @return Status of the operation.
         */
        int setConfigFilter(const uint8_t filter);

        /**
         * @brief Sets the measurement control register.
         * @param ctrl_measure Control measurement value.
         * @return Status of the operation.
         */
        int setCtrlMeas(const uint8_t ctrl_measure);

        /**
         * @brief Configures the temperature oversampling.
         * @param temperature_oversampling Oversampling setting for temperature.
         * @return Status of the operation.
         */
        int setTemperatureOversampling(const uint8_t temperature_oversampling);

        /**
         * @brief Configures the pressure oversampling.
         * @param pressure_oversampling Oversampling setting for pressure.
         * @return Status of the operation.
         */
        int setPressureOversampling(const uint8_t pressure_oversampling);

        /**
         * @brief Configures both temperature and pressure oversampling.
         * @param temperature_oversampling Oversampling setting for temperature.
         * @param pressure_oversampling Oversampling setting for pressure.
         * @return Status of the operation.
         */
        int setOversampling(const uint8_t temperature_oversampling, const uint8_t pressure_oversampling);

        /**
         * @brief Sets the sensor operating mode.
         * @param mode Operating mode value.
         * @return Status of the operation.
         */
        int setMode(const uint8_t mode);

        /**
         * @brief Configures the humidity oversampling.
         * @param humididtyOversampling Oversampling setting for humidity.
         * @return Status of the operation.
         */
        int setCtrlHum(const int humididtyOversampling);

        /**
         * @brief Retrieves all sensor results in a result structure.
         * @param results Pointer to the result structure.
         * @return Status of the operation.
         */
        int getAllResults(BME280ResultData *results);

        /**
         * @brief Retrieves all sensor results as individual values.
         * @param temperature Pointer to store temperature value.
         * @param humidity Pointer to store humidity value.
         * @param pressure Pointer to store pressure value.
         * @return Status of the operation.
         */
        int getAllResults(float *temperature, int *humidity, float *pressure);

        /**
         * @brief Retrieves the temperature value.
         * @return Compensated temperature in degrees Celsius.
         */
        float getTemperature();

        /**
         * @brief Retrieves the pressure value.
         * @return Compensated pressure in hPa.
         */
        float getPressure();

        /**
         * @brief Retrieves the humidity value.
         * @return Compensated humidity in percentage.
         */
        int getHumidity();

        /**
         * @brief Checks if the sensor is currently measuring.
         * @return True if the sensor is busy, false otherwise.
         */
        bool statusMeasuringBusy();

        /**
         * @brief Checks if an NVM update is in progress.
         * @return True if an update is in progress, false otherwise.
         */
        bool imUpdateBusy();

        /**
         * @brief Resets the sensor.
         * @return Status of the operation.
         */
        int reset();
    };
} 