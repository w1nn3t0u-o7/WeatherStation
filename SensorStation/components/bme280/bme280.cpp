#include "bme280.hpp"

namespace MZDK {
    BME280::BME280(ComProtocol *com_protocol, 
                    const uint8_t humidity_oversampling,
                    const uint8_t temperature_oversampling,
                    const uint8_t pressure_oversampling,
                    const uint8_t sensor_mode) 
                    : m_com_protocol(com_protocol), 
                    m_humidity_oversampling_value(humidity_oversampling),
                    m_temperature_oversampling_value(temperature_oversampling),
                    m_pressure_oversampling_value(pressure_oversampling),
                    m_sensor_mode_value(sensor_mode) {
    
        m_com_protocol->writeRegister(CONFIG, 0); // Enable SPI 4-wire
        getCalibrateData();
        m_com_protocol->writeRegister(CTRL_HUM, m_humidity_oversampling_value);
        m_com_protocol->writeRegister(CTRL_MEAS, m_pressure_oversampling_value | m_temperature_oversampling_value | m_sensor_mode_value);
    };
    
    int BME280::getStatus() {
        return m_com_protocol->readRegister(STATUS);
    }

    int BME280::getCalibrateData() {
        // Temperature calibration data 
        dig_t1 = static_cast<unsigned short>(m_com_protocol->read2Registers(0x88));
        dig_t2 = static_cast<signed short>(m_com_protocol->read2Registers(0x8A));
        dig_t3 = static_cast<signed short>(m_com_protocol->read2Registers(0x8C));
        // Pressure calibration data 
        dig_p1 = static_cast<unsigned short>(m_com_protocol->read2Registers(0x8E));
        dig_p2 = static_cast<signed short>(m_com_protocol->read2Registers(0x90));
        dig_p3 = static_cast<signed short>(m_com_protocol->read2Registers(0x92));
        dig_p4 = static_cast<signed short>(m_com_protocol->read2Registers(0x94));
        dig_p5 = static_cast<signed short>(m_com_protocol->read2Registers(0x96));
        dig_p6 = static_cast<signed short>(m_com_protocol->read2Registers(0x98));
        dig_p7 = static_cast<signed short>(m_com_protocol->read2Registers(0x9A));
        dig_p8 = static_cast<signed short>(m_com_protocol->read2Registers(0x9C));
        dig_p9 = static_cast<signed short>(m_com_protocol->read2Registers(0x9E));
        // Humidity calibration data 
        dig_h1 = static_cast<unsigned char>(m_com_protocol->readRegister(0xA1));
        dig_h2 = static_cast<signed short>(m_com_protocol->read2Registers(0xE1));
        dig_h3 = static_cast<unsigned char>(m_com_protocol->readRegister(0xE3));
        int8_t digH4Msb = static_cast<int8_t>(m_com_protocol->readRegister(0xE4));
        int8_t digH4H5Shared = static_cast<int8_t>(m_com_protocol->readRegister(0xE5)); // this register hold parts of the values of dig_H4 and dig_h5
        int8_t digH5Msb = static_cast<int8_t>(m_com_protocol->readRegister(0xE6));
        dig_h6 = static_cast<int8_t>(m_com_protocol->readRegister(0xE7));

        dig_h4 = static_cast<signed short>(digH4Msb << 4 | (digH4H5Shared & 0x0F));        // split and shift the bits appropriately.
        dig_h5 = static_cast<signed short>(digH5Msb << 4 | ((digH4H5Shared & 0xF0) >> 4)); // split and shift the bits appropriately.

        return 0;
    }

    int BME280::getSensorData(m_SensorRawData *resultRaw) {
        uint8_t result_buf[8];

        if (m_sensor_mode_value == sensor_forced_mode)
        {
            setMode(sensor_forced_mode);
            while (statusMeasuringBusy() || imUpdateBusy())
            {
                vTaskDelay(pdMS_TO_TICKS(50));
            }
        }

        m_com_protocol->readRegisterMultipleBytes(PRESS_MSB, result_buf, 8);

        uint8_t press_msb = result_buf[0];
        uint8_t press_lsb = result_buf[1];
        uint8_t press_xlsb = result_buf[2];
        uint8_t temp_msb = result_buf[3];
        uint8_t temp_lsb = result_buf[4];
        uint8_t temp_xlsb = result_buf[5];
        uint8_t hum_msb = result_buf[6];
        uint8_t hum_lsb = result_buf[7];

        resultRaw->m_temperature = temp_msb << 12 | temp_lsb << 4 | temp_xlsb >> 4;
        resultRaw->m_pressure = press_msb << 12 | press_lsb << 4 | press_xlsb >> 4;
        resultRaw->m_humididty = hum_msb << 8 | hum_lsb;

        return 0;
    }

    float BME280::compensateTemp(const signed long adc_T) {
        int32_t var1;
        int32_t var2;
        int32_t temperature;
        int32_t temperature_min = -4000;
        int32_t temperature_max = 8500;

        var1 = (int32_t)((adc_T / 8) - ((int32_t)dig_t1 * 2));
        var1 = (var1 * ((int32_t)dig_t2)) / 2048;
        var2 = (int32_t)((adc_T / 16) - ((int32_t)dig_t1));
        var2 = (((var2 * var2) / 4096) * ((int32_t)dig_t3)) / 16384;
        t_fine = var1 + var2;
        temperature = (t_fine * 5 + 128) / 256;

        if (temperature < temperature_min)
        {
            temperature = temperature_min;
        }
        else if (temperature > temperature_max)
        {
            temperature = temperature_max;
        }

        float returnTemperature = temperature;

        return returnTemperature / 100;
    }

    float BME280::compensatePressure(const unsigned long adc_P) {
        int64_t var1;
        int64_t var2;
        int64_t var3;
        int64_t var4;
        uint32_t pressure;
        uint32_t pressure_min = 3000000;
        uint32_t pressure_max = 11000000;

        var1 = ((int64_t)t_fine) - 128000;
        var2 = var1 * var1 * (int64_t)dig_p6;
        var2 = var2 + ((var1 * (int64_t)dig_p5) * 131072);
        var2 = var2 + (((int64_t)dig_p4) * 34359738368);
        var1 = ((var1 * var1 * (int64_t)dig_p3) / 256) + ((var1 * ((int64_t)dig_p2) * 4096));
        var3 = ((int64_t)1) * 140737488355328;
        var1 = (var3 + var1) * ((int64_t)dig_p1) / 8589934592;

        /* To avoid divide by zero exception */
        if (var1 != 0)
        {
            var4 = 1048576 - adc_P;
            var4 = (((var4 * INT64_C(2147483648)) - var2) * 3125) / var1;
            var1 = (((int64_t)dig_p9) * (var4 / 8192) * (var4 / 8192)) / 33554432;
            var2 = (((int64_t)dig_p8) * var4) / 524288;
            var4 = ((var4 + var1 + var2) / 256) + (((int64_t)dig_p7) * 16);
            pressure = (uint32_t)(((var4 / 2) * 100) / 128);

            if (pressure < pressure_min)
            {
                pressure = pressure_min;
            }
            else if (pressure > pressure_max)
            {
                pressure = pressure_max;
            }
        }
        else
        {
            pressure = pressure_min;
        }

        float returnPressure = pressure;

        return returnPressure / 100;
    }

    int BME280::compensateHumidity(const unsigned long adc_H) {
        int32_t var1;
        int32_t var2;
        int32_t var3;
        int32_t var4;
        int32_t var5;
        uint32_t humidity;
        uint32_t humidity_max = 102400;

        var1 = t_fine - ((int32_t)76800);
        var2 = (int32_t)(adc_H * 16384);
        var3 = (int32_t)(((int32_t)dig_h4) * 1048576);
        var4 = ((int32_t)dig_h5) * var1;
        var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
        var2 = (var1 * ((int32_t)dig_h6)) / 1024;
        var3 = (var1 * ((int32_t)dig_h3)) / 2048;
        var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
        var2 = ((var4 * ((int32_t)dig_h2)) + 8192) / 16384;
        var3 = var5 * var2;
        var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
        var5 = var3 - ((var4 * ((int32_t)dig_h1)) / 16);
        var5 = (var5 < 0 ? 0 : var5);
        var5 = (var5 > 419430400 ? 419430400 : var5);
        humidity = (uint32_t)(var5 / 4096);

        if (humidity > humidity_max)
        {
            humidity = humidity_max;
        }

        return humidity/1024;
    }

    int BME280::getDeviceID() {
        return m_com_protocol->readRegister(ID);
    }

    int BME280::setConfig(const uint8_t config) {
        return m_com_protocol->writeRegister(CONFIG, config);
    }

    int BME280::setConfigStandbyT(const uint8_t standby) {// config bits 7, 6, 5  page 30
        uint8_t temp = m_com_protocol->readRegister(CONFIG) & 0b00011111;

        return m_com_protocol->writeRegister(CONFIG, temp | standby);
    }

    int BME280::setConfigFilter(const uint8_t filter) {// config bits 4, 3, 2
        uint8_t temp = m_com_protocol->readRegister(CONFIG);
        temp = temp & 0b11100011;
        temp = temp | filter << 2;

        return m_com_protocol->writeRegister(CONFIG, temp);
    }

    int BME280::setCtrlMeas(const uint8_t ctrl_measure) {
        m_pressure_oversampling_value = 0 | (ctrl_measure & 0b11100011);
        m_temperature_oversampling_value = 0 | (ctrl_measure & 0b00011111);
        m_sensor_mode_value = 0 | (ctrl_measure & 0b11111100);

        return m_com_protocol->writeRegister(CTRL_MEAS, ctrl_measure);
    }

    int BME280::setTemperatureOversampling(const uint8_t temperature_oversampling) {// ctrl_meas bits 7, 6, 5   page 29
        uint8_t temp = m_com_protocol->readRegister(CTRL_MEAS) & 0b00011111;
        m_temperature_oversampling_value = temperature_oversampling;

        return m_com_protocol->writeRegister(CTRL_MEAS, temp | temperature_oversampling);
    }

    int BME280::setPressureOversampling(const uint8_t pressure_oversampling) {// ctrl_meas bits 4, 3, 2
        uint8_t temp = m_com_protocol->readRegister(CTRL_MEAS) & 0b11100011;
        m_pressure_oversampling_value = pressure_oversampling;

        return m_com_protocol->writeRegister(CTRL_MEAS, temp | pressure_oversampling);
    }

    int BME280::setOversampling(const uint8_t temperature_oversampling, const uint8_t pressure_oversampling) {
        m_pressure_oversampling_value = 0 | pressure_oversampling;
        m_temperature_oversampling_value = 0 | temperature_oversampling;

        return m_com_protocol->writeRegister(CTRL_MEAS, temperature_oversampling | pressure_oversampling | m_sensor_mode_value);
    }

    int BME280::setMode(const uint8_t mode) {// ctrl_meas bits 1, 0
        uint8_t temp = m_com_protocol->readRegister(CTRL_MEAS) & 0b11111100;
        m_sensor_mode_value = mode;

        return m_com_protocol->writeRegister(CTRL_MEAS, temp | mode);
    }

    int BME280::setCtrlHum(const int humididty_oversampling) {// ctrl_hum bits 2, 1, 0    page 28
        m_humidity_oversampling_value = humididty_oversampling;
        
        return m_com_protocol->writeRegister(CTRL_HUM, humididty_oversampling);
    }

    int BME280::getAllResults(BME280ResultData *results) {
        m_SensorRawData resultRaw{};

        getSensorData(&resultRaw);

        results->temperature = compensateTemp(resultRaw.m_temperature);
        results->humididty = compensateHumidity(resultRaw.m_humididty);
        results->pressure = compensatePressure(resultRaw.m_pressure);

        return 0;
    }

    int BME280::getAllResults(float *temperature, int *humidity, float *pressure) {
        m_SensorRawData result_raw{};

        getSensorData(&result_raw);

        *temperature = compensateTemp(result_raw.m_temperature);
        *humidity = compensateHumidity(result_raw.m_humididty);
        *pressure = compensatePressure(result_raw.m_pressure);

        return 0;
    }

    float BME280::getTemperature() {// Preferable to use GetAllResults()
        BME280ResultData results{};

        getAllResults(&results);

        return results.temperature; // compensateTemp(resultRaw.temperature);
    }

    float BME280::getPressure() {
        BME280ResultData results{};

        getAllResults(&results);

        return results.pressure;
    }

    int BME280::getHumidity() {
        BME280ResultData results{};

        getAllResults(&results);

        return results.humididty;
    }

    bool BME280::statusMeasuringBusy() {// check status (0xF3) bit 3
        return ((m_com_protocol->readRegister(STATUS) & 8) == 8) ? true : false;
    }

    bool BME280::imUpdateBusy() {// check status (0xF3) bit 0
        return ((m_com_protocol->readRegister(STATUS) & 1) == 1) ? true : false;
    }
    
    int BME280::reset() {// write 0xB6 into reset (0xE0)
        return m_com_protocol->writeRegister(RESET, 0xB6);
    }
} 
