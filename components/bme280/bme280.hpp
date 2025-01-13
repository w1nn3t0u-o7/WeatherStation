#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//#include "i2c.hpp"
#include "spi.hpp"

/**
 * @file BME280.hpp
 * @brief This file contains the interface for the BME280 sensor, including configuration and calibration methods, 
 *       and provides a way to access temperature, humidity, and pressure data.
 */

/**
 * @brief BME280 Sensor Register Addresses
 * 
 * The following are the register addresses used to communicate with the BME280 sensor. These registers
 * hold raw data and configuration settings for the sensor.
 */
constexpr static uint8_t HUM_LSB = 0xFE;    ///< Humidity LSB Register
constexpr static uint8_t HUM_MSB = 0xFD;    ///< Humidity MSB Register
constexpr static uint8_t TEMP_XLSB = 0xFC;  ///< Temperature XLSB Register
constexpr static uint8_t TEMP_LSB = 0xFB;   ///< Temperature LSB Register
constexpr static uint8_t TEMP_MSB = 0xFA;   ///< Temperature MSB Register
constexpr static uint8_t PRESS_XLSB = 0xF9; ///< Pressure XLSB Register
constexpr static uint8_t PRESS_LSB = 0xF8;  ///< Pressure LSB Register
constexpr static uint8_t PRESS_MSB = 0xF7;  ///< Pressure MSB Register
constexpr static uint8_t CONFIG = 0xF5;     ///< Configuration Register
constexpr static uint8_t CTRL_MEAS = 0xF4;  ///< Control Measurement Register
constexpr static uint8_t STATUS = 0xF3;     ///< Status Register
constexpr static uint8_t CTRL_HUM = 0xF2;   ///< Control Humidity Register
constexpr static uint8_t RESET = 0xE0;      ///< Reset Register
constexpr static uint8_t ID = 0xD0;         ///< Device ID Register

/**
 * @brief BME280 Sensor Settings
 * 
 * The following are various configuration settings for the BME280 sensor, such as oversampling rates,
 * sensor modes, and filter settings.
 */
constexpr static uint8_t pressureSensorDisable = 0x00 << 2;
constexpr static uint8_t pressureOversamplingX1 = 0x01 << 2;
constexpr static uint8_t pressureOversamplingX2 = 0x02 << 2;
constexpr static uint8_t pressureOversamplingX4 = 0x03 << 2;
constexpr static uint8_t pressureOversamplingX8 = 0x04 << 2;
constexpr static uint8_t pressureOversamplingX16 = 0x05 << 2;
constexpr static uint8_t temperatureSensorDisable = 0x00 << 5;
constexpr static uint8_t temperatureOversamplingX1 = 0x01 << 5;
constexpr static uint8_t temperatureOversamplingX2 = 0x02 << 5;
constexpr static uint8_t temperatureOversamplingX4 = 0x03 << 5;
constexpr static uint8_t temperatureOversamplingX8 = 0x04 << 5;
constexpr static uint8_t temperatureOversamplingX16 = 0x05 << 5;
constexpr static uint8_t sensorSleepMode = 0x00;
constexpr static uint8_t sensorForcedMode = 0x01;
constexpr static uint8_t sensorNormalMode = 0x03;

constexpr static uint8_t configStandby0_5ms = 0x00 << 5;
constexpr static uint8_t configStandby62_5ms = 0x01 << 5;
constexpr static uint8_t configStandby125ms = 0x02 << 5;
constexpr static uint8_t configStandby250ms = 0x03 << 5;
constexpr static uint8_t configStandby500ms = 0x04 << 5;
constexpr static uint8_t configStandby1000ms = 0x05 << 5;
constexpr static uint8_t configStandby10ms = 0x06 << 5;
constexpr static uint8_t configStandby20ms = 0x07 << 5;
constexpr static uint8_t configFilterOff = 0x00 << 2;
constexpr static uint8_t configFilter2 = 0x01 << 2;
constexpr static uint8_t configFilter4 = 0x02 << 2;
constexpr static uint8_t configFilter8 = 0x03 << 2;
constexpr static uint8_t configFilter16 = 0x04 << 2;

constexpr static uint8_t humiditySensorDisable = 0x00;
constexpr static uint8_t humidityOversamplingX1 = 0x01;
constexpr static uint8_t humidityOversamplingX2 = 0x02;
constexpr static uint8_t humidityOversamplingX4 = 0x03;
constexpr static uint8_t humidityOversamplingX8 = 0x04;
constexpr static uint8_t humidityOversamplingX16 = 0x05;

namespace MZDK {
    /**
     * @class BME280
     * @brief Interface to the BME280 sensor that handles reading temperature, pressure, and humidity data.
     * 
     * This class provides functions to configure the BME280 sensor, read sensor data, and compensate raw
     * data for accurate readings. It also provides methods for checking the sensor status and controlling its
     * operating mode.
     */
    class BME280 {
    private:
        ComProtocol *m_com_protocol; ///< Communication protocol object (e.g., SPI or I2C)
        
        /**
         * @brief Struct to store raw sensor data.
         */
        struct m_SensorRawData {
            long m_temperature = 0;      ///< Raw temperature data
            unsigned long m_humididty = 0; ///< Raw humidity data
            unsigned long m_pressure = 0;  ///< Raw pressure data
        };

        uint8_t m_humidity_oversampling; ///< Humidity oversampling setting
        uint8_t m_temperature_oversampling; ///< Temperature oversampling setting
        uint8_t m_pressure_oversampling; ///< Pressure oversampling setting
        uint8_t m_sensor_mode; ///< Sensor operation mode (sleep, forced, normal)

        // Calibration data (from the sensor)
        unsigned short  dig_t1 = 0; ///< Calibration temperature data
        signed short    dig_t2 = 0; ///< Calibration temperature data
        signed short    dig_t3 = 0; ///< Calibration temperature data
        signed long     t_fine = 0; ///< Fine temperature calibration data
        unsigned short  dig_p1 = 0; ///< Calibration pressure data
        signed short    dig_p2 = 0; ///< Calibration pressure data
        signed short    dig_p3 = 0; ///< Calibration pressure data
        signed short    dig_p4 = 0; ///< Calibration pressure data
        signed short    dig_p5 = 0; ///< Calibration pressure data
        signed short    dig_p6 = 0; ///< Calibration pressure data
        signed short    dig_p7 = 0; ///< Calibration pressure data
        signed short    dig_p8 = 0; ///< Calibration pressure data
        signed short    dig_p9 = 0; ///< Calibration pressure data
        uint8_t         dig_h1 = 0; ///< Calibration humidity data
        signed short    dig_h2 = 0; ///< Calibration humidity data
        uint8_t         dig_h3 = 0; ///< Calibration humidity data
        signed short    dig_h4 = 0; ///< Calibration humidity data
        signed short    dig_h5 = 0; ///< Calibration humidity data
        signed char     dig_h6 = 0; ///< Calibration humidity data

        /**
         * @brief Gets the current sensor status.
         * 
         * @return Status code indicating the current sensor state.
         */
        int getStatus();

        /**
         * @brief Retrieves the calibration data from the sensor.
         * 
         * @return Status code indicating the result of the operation.
         */
        int getCalibrateData();

        /**
         * @brief Retrieves the raw sensor data (temperature, pressure, humidity).
         * 
         * @param resultRaw Pointer to a structure where the raw sensor data will be stored.
         * @return Status code indicating the result of the operation.
         */
        int getSensorData(m_SensorRawData *resultRaw);

        /**
         * @brief Compensates the raw temperature data.
         * 
         * @param adc_T Raw temperature data.
         * @return Compensated temperature value in Celsius.
         */
        float compensateTemp(const signed long adc_T);

        /**
         * @brief Compensates the raw pressure data.
         * 
         * @param adc_P Raw pressure data.
         * @return Compensated pressure value in hPa.
         */
        float compensatePressure(const unsigned long adc_P);

        /**
         * @brief Compensates the raw humidity data.
         * 
         * @param adc_H Raw humidity data.
         * @return Compensated humidity value in percentage.
         */
        int compensateHumidity(const unsigned long adc_H);

    public:
        /**
         * @brief Constructor to initialize the BME280 object.
         * 
         * @param com_protocol Communication protocol object (e.g., SPI or I2C).
         */
        BME280(ComProtocol *com_protocol);

        /**
         * @brief Constructor to initialize the BME280 object with custom settings.
         * 
         * @param com_protocol Communication protocol object (e.g., SPI or I2C).
         * @param humidity_oversampling Oversampling rate for humidity.
         * @param temperature_oversampling Oversampling rate for temperature.
         * @param pressure_oversampling Oversampling rate for pressure.
         * @param sensor_mode Operating mode of the sensor.
         */
        BME280(ComProtocol *com_protocol, 
               uint8_t humidity_oversampling,
               uint8_t temperature_oversampling,
               uint8_t pressure_oversampling,
               uint8_t sensor_mode);

        void Init();

        /**
         * @brief Struct to store the results from the BME280 sensor.
         */
        struct BME280ResultData {
            float temperature = 0.0; ///< Compensated temperature
            int humididty = 0; ///< Compensated humidity
            float pressure = 0.0; ///< Compensated pressure
        } results;

        /**
         * @brief Retrieves the device ID.
         * 
         * @return Device ID.
         */
        int GetDeviceID(void);

        /**
         * @brief Sets the sensor configuration register.
         * 
         * @param config Configuration settings.
         * @return Status code indicating the result of the operation.
         */
        int SetConfig(const uint8_t config);

        /**
         * @brief Sets the standby time in the configuration register.
         * 
         * @param standby Standby time setting.
         * @return Status code indicating the result of the operation.
         */
        int SetConfigStandbyT(const uint8_t standby);

        /**
         * @brief Sets the filter setting in the configuration register.
         * 
         * @param filter Filter setting.
         * @return Status code indicating the result of the operation.
         */
        int SetConfigFilter(const uint8_t filter);

        /**
         * @brief Sets the control measurement register for temperature and pressure settings.
         * 
         * @param ctrlMeas Control measurement setting.
         * @return Status code indicating the result of the operation.
         */
        int SetCtrlMeas(const uint8_t ctrlMeas);

        /**
         * @brief Sets the temperature oversampling rate.
         * 
         * @param tempOversampling Temperature oversampling setting.
         * @return Status code indicating the result of the operation.
         */
        int SetTemperatureOversampling(const uint8_t tempOversampling);

        /**
         * @brief Sets the pressure oversampling rate.
         * 
         * @param pressureOversampling Pressure oversampling setting.
         * @return Status code indicating the result of the operation.
         */
        int SetPressureOversampling(const uint8_t pressureOversampling);

        /**
         * @brief Sets both temperature and pressure oversampling rates.
         * 
         * @param tempOversampling Temperature oversampling setting.
         * @param pressureOversampling Pressure oversampling setting.
         * @return Status code indicating the result of the operation.
         */
        int SetOversampling(const uint8_t tempOversampling, const uint8_t pressureOversampling);

        /**
         * @brief Sets the sensor mode (sleep, forced, normal).
         * 
         * @param mode Sensor mode setting.
         * @return Status code indicating the result of the operation.
         */
        int SetMode(const uint8_t mode);

        /**
         * @brief Sets the humidity oversampling rate.
         * 
         * @param humidityOversampling Humidity oversampling setting.
         * @return Status code indicating the result of the operation.
         */
        int SetCtrlHum(const int humididtyOversampling);

        /**
         * @brief Retrieves all the sensor results (temperature, humidity, pressure).
         * 
         * @param results Pointer to a structure where the sensor results will be stored.
         * @return Status code indicating the result of the operation.
         */
        int GetAllResults(BME280ResultData *results);

        /**
         * @brief Retrieves the current temperature.
         * 
         * @return Compensated temperature in Celsius.
         */
        float GetTemperature(void);

        /**
         * @brief Retrieves the current pressure.
         * 
         * @return Compensated pressure in hPa.
         */
        float GetPressure(void);

        /**
         * @brief Retrieves the current humidity.
         * 
         * @return Compensated humidity as a percentage.
         */
        int GetHumidity(void);

        /**
         * @brief Checks if the sensor is busy with measurements.
         * 
         * @return True if the sensor is measuring, false otherwise.
         */
        bool StatusMeasuringBusy(void);

        /**
         * @brief Checks if the sensor is updating its data.
         * 
         * @return True if the sensor is updating, false otherwise.
         */
        bool ImUpdateBusy(void);

        /**
         * @brief Resets the sensor.
         * 
         * @return Status code indicating the result of the operation.
         */
        int Reset(void);
    };
}