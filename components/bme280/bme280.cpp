#include <memory>
#include "bme280.hpp"

MZDK::BME280::BME280(ComProtocol *com_protocol) {
    m_humidity_oversampling = humidityOversamplingX1;
    m_temperature_oversampling = temperatureOversamplingX1;
    m_pressure_oversampling = pressureOversamplingX1;
    m_sensor_mode = sensorForcedMode;

    com_protocol->write(CONFIG, 0); // Enable SPI 4-wire
    getCalibrateData();
    com_protocol->write(CTRL_HUM, m_humidity_oversampling);
    com_protocol->write(CTRL_MEAS, m_pressure_oversampling | m_temperature_oversampling | m_sensor_mode);
}
MZDK::BME280::BME280(ComProtocol *com_protocol, uint8_t humidity_oversampling, uint8_t temperature_oversampling, uint8_t pressure_oversampling, uint8_t sensor_mode) :
                    m_humidity_oversampling(humidity_oversampling), 
                    m_temperature_oversampling(temperature_oversampling),
                    m_pressure_oversampling(pressure_oversampling),
                    m_sensor_mode(sensor_mode) {

    com_protocol->write(CONFIG, 0); // Enable SPI 4-wire
    getCalibrateData();
    com_protocol->write(CTRL_HUM, m_humidity_oversampling);
    com_protocol->write(CTRL_MEAS, m_pressure_oversampling | m_temperature_oversampling | m_sensor_mode);

}

    int MZDK::BME280::getStatus() {
        return com_protocol->read(STATUS);
    }

    int MZDK::BME280::getCalibrateData() {
        //============================== Temperature Calibration Data ===========================
        dig_t1 = com_protocol->read(0x88);
        dig_t2 = com_protocol->read(0x8A);
        dig_t3 = com_protocol->read(0x8C);
        //=======================================================================================
        //============================== Pressure Calibration Data ==============================
        dig_p1 = com_protocol->read(0x8E);
        dig_p2 = com_protocol->read(0x90);
        dig_p3 = com_protocol->read(0x92);
        dig_p4 = com_protocol->read(0x94);
        dig_p5 = com_protocol->read(0x96);
        dig_p6 = com_protocol->read(0x98);
        dig_p7 = com_protocol->read(0x9A);
        dig_p8 = com_protocol->read(0x9C);
        dig_p9 = com_protocol->read(0x9E);
        //=======================================================================================
        //============================== Humidity Calibration Data ==============================
        dig_h1 = com_protocol->read(0xA1);
        dig_h2 = com_protocol->read(0xE1);
        dig_h3 = com_protocol->read(0xE3);
        int8_t digH4Msb = com_protocol->read(0xE4);
        int8_t digH4H5Shared = com_protocol->read(0xE5); // this register hold parts of the values of dig_H4 and dig_h5
        int8_t digH5Msb = com_protocol->read(0xE6);
        dig_h6 = com_protocol->read(0xE7);

        dig_h4 = static_cast<signed short>(digH4Msb << 4 | (digH4H5Shared & 0x0F));        // split and shift the bits appropriately.
        dig_h5 = static_cast<signed short>(digH5Msb << 4 | ((digH4H5Shared & 0xF0) >> 4)); // split and shift the bits appropriately.
        //=======================================================================================

        return 0;
    }

    int MZDK::BME280::getSensorData(m_SensorRawData *resultRaw) {

        uint8_t pressMsb = com_protocol->read(PRESS_MSB);
        uint8_t pressLsb = com_protocol->read(PRESS_LSB);
        uint8_t pressXlsb = com_protocol->read(PRESS_XLSB);
        uint8_t tempMsb = com_protocol->read(TEMP_MSB);
        uint8_t tempLsb = com_protocol->read(TEMP_LSB);
        uint8_t tempXlsb = com_protocol->read(TEMP_XLSB);
        uint8_t humMsb = com_protocol->read(HUM_MSB);
        uint8_t humLsb = com_protocol->read(HUM_LSB);

        resultRaw->m_temperature = tempMsb << 12 | tempLsb << 4 | tempXlsb >> 4;
        resultRaw->m_pressure = pressMsb << 12 | pressLsb << 4 | pressXlsb >> 4;
        resultRaw->m_humididty = humMsb << 8 | humLsb;

        return 0;
    }

    float MZDK::BME280::compensateTemp(const signed long adc_T) {
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

        if (temperature < temperature_min) {
            temperature = temperature_min;
        }
        else if (temperature > temperature_max) {
            temperature = temperature_max;
        }

        float returnTemperature = temperature;

        return returnTemperature / 100;
    }

    float MZDK::BME280::compensatePressure(const unsigned long adc_P) {
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
        if (var1 != 0) {
            var4 = 1048576 - adc_P;
            var4 = (((var4 * INT64_C(2147483648)) - var2) * 3125) / var1;
            var1 = (((int64_t)dig_p9) * (var4 / 8192) * (var4 / 8192)) / 33554432;
            var2 = (((int64_t)dig_p8) * var4) / 524288;
            var4 = ((var4 + var1 + var2) / 256) + (((int64_t)dig_p7) * 16);
            pressure = (uint32_t)(((var4 / 2) * 100) / 128);

            if (pressure < pressure_min) {
                pressure = pressure_min;
            }
            else if (pressure > pressure_max) {
                pressure = pressure_max;
            }
        }
        else {
            pressure = pressure_min;
        }

        float returnPressure = pressure;

        return returnPressure / 100;
    }

    int MZDK::BME280::compensateHumidity(const unsigned long adc_H) {
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

        if (humidity > humidity_max) {
            humidity = humidity_max;
        }

        return humidity/1024;
    }

    int MZDK::BME280::GetDeviceID() {
        return com_protocol->read(ID);
    }

    int MZDK::BME280::SetConfig(const uint8_t config) {
        return com_protocol->write(CONFIG, config);
    }

    int MZDK::BME280::SetConfigStandbyT(const uint8_t standby) { // config bits 7, 6, 5  page 30
        uint8_t temp = com_protocol->read(CONFIG) & 0b00011111;

        return com_protocol->write(CONFIG, temp | standby);
    }

    int MZDK::BME280::SetConfigFilter(const uint8_t filter) { // config bits 4, 3, 2
        uint8_t temp = com_protocol->read(CONFIG);
        temp = temp & 0b11100011;
        temp = temp | filter << 2;

        return com_protocol->write(CONFIG, temp);
    }

    int MZDK::BME280::SetCtrlMeas(const uint8_t ctrlMeas) {
        m_pressure_oversampling = 0 | (ctrlMeas & 0b11100011);
        m_temperature_oversampling = 0 | (ctrlMeas & 0b00011111);
        m_sensor_mode = 0 | (ctrlMeas & 0b11111100);

        return com_protocol->write(CTRL_MEAS, ctrlMeas);
    }

    int MZDK::BME280::SetTemperatureOversampling(const uint8_t tempOversampling) { // ctrl_meas bits 7, 6, 5   page 29
        uint8_t temp = com_protocol->read(CTRL_MEAS) & 0b00011111;
        m_temperature_oversampling = tempOversampling;

        return com_protocol->write(CTRL_MEAS, temp | tempOversampling);
    }

    int MZDK::BME280::SetPressureOversampling(const uint8_t pressureOversampling) { // ctrl_meas bits 4, 3, 2
        uint8_t temp = com_protocol->read(CTRL_MEAS) & 0b11100011;
        m_pressure_oversampling = pressureOversampling;

        return com_protocol->write(CTRL_MEAS, temp | pressureOversampling);
    }

    int MZDK::BME280::SetOversampling(const uint8_t tempOversampling, const uint8_t pressureOversampling) {
        m_pressure_oversampling = 0 | pressureOversampling;
        m_temperature_oversampling = 0 | tempOversampling;

        return com_protocol->write(CTRL_MEAS, tempOversampling | pressureOversampling | m_sensor_mode);
    }

    int MZDK::BME280::SetMode(const uint8_t mode) { // ctrl_meas bits 1, 0
        uint8_t temp = com_protocol->read(CTRL_MEAS) & 0b11111100;
        m_sensor_mode = mode;

        return com_protocol->write(CTRL_MEAS, temp | mode);
    }

    int MZDK::BME280::SetCtrlHum(const int humididtyOversampling) { // ctrl_hum bits 2, 1, 0    page 28
        m_humidity_oversampling = humididtyOversampling;
        
        return com_protocol->write(CTRL_HUM, humididtyOversampling);
    }

    int MZDK::BME280::GetAllResults(BME280ResultData *results) {
        m_SensorRawData resultRaw{};

        getSensorData(&resultRaw);

        results->temperature = compensateTemp(resultRaw.m_temperature);
        results->humididty = compensateHumidity(resultRaw.m_humididty);
        results->pressure = compensatePressure(resultRaw.m_pressure);

        return 0;
    }

    float MZDK::BME280::GetTemperature(void) { // Preferable to use GetAllResults()
        BME280ResultData results{};

        GetAllResults(&results);

        return results.temperature; // compensateTemp(resultRaw.temperature);
    }

    float MZDK::BME280::GetPressure(void) {
        BME280ResultData results{};

        GetAllResults(&results);

        return results.pressure;
    }

    int MZDK::BME280::GetHumidity(void) {
        BME280ResultData results{};

        GetAllResults(&results);

        return results.humididty;
    }

    bool MZDK::BME280::StatusMeasuringBusy(void) { // check status (0xF3) bit 3
        return ((com_protocol->read(STATUS) & 8) == 8) ? true : false;
    }

    bool MZDK::BME280::ImUpdateBusy(void) { // check status (0xF3) bit 0
        return ((com_protocol->read(STATUS) & 1) == 1) ? true : false;
    }
    
    int MZDK::BME280::Reset(void) { // write 0xB6 into reset (0xE0)
        return com_protocol->write(RESET, 0xB6);
    }
