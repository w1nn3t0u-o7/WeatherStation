#include "spi.hpp"

namespace MZDK {
    SPI::SPI(const spi_host_device_t spi_peripheral, const int miso_pin, const int mosi_pin, const int sclk_pin) {
        m_spi_peripheral = spi_peripheral;

        m_spi_transfer.tx_buffer = nullptr;
        m_spi_transfer.rx_buffer = nullptr;

        m_spi_bus_config.mosi_io_num = mosi_pin;
        m_spi_bus_config.miso_io_num = miso_pin;
        m_spi_bus_config.sclk_io_num = sclk_pin;
        m_spi_bus_config.quadwp_io_num = -1;
        m_spi_bus_config.quadhd_io_num = -1;

        spi_bus_initialize(spi_peripheral, &m_spi_bus_config, SPI_DMA_CH_AUTO);
    }

    int SPI::m_transferByte(const uint8_t reg_addr, const uint8_t data, const uint8_t command) {
        m_spi_transfer.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
        m_spi_transfer.cmd = command;
        m_spi_transfer.length = 8;
        m_spi_transfer.addr = reg_addr;
        m_spi_transfer.tx_data[0] = data;

        return spi_device_transmit(m_spi_device_handle, &m_spi_transfer);
    }

    int SPI::m_transferMultiplesBytes(const uint8_t reg_addr, uint8_t* tx_buf, uint8_t* rx_buf, size_t data_length, const uint8_t command) {
        spi_transaction_t spi_transaction_multibyte;        

        if (data_length < 1) { 
            data_length = 1; 
        }

        spi_transaction_multibyte.flags = 0;
        spi_transaction_multibyte.length = (8 * data_length);
        spi_transaction_multibyte.rxlength = 0;
        spi_transaction_multibyte.cmd = command;
        spi_transaction_multibyte.addr = reg_addr;
        spi_transaction_multibyte.tx_buffer = tx_buf;
        spi_transaction_multibyte.rx_buffer = rx_buf;        

        return spi_device_transmit(m_spi_device_handle, &spi_transaction_multibyte);
    }

    int SPI::registerDevice(const uint8_t mode, const int cs, const int addr_length, const int command_length, const int bus_speed) {

        m_spi_interface_config = {};
        m_spi_interface_config.command_bits = command_length; 
        m_spi_interface_config.address_bits = addr_length;
        m_spi_interface_config.mode = mode;
        m_spi_interface_config.clock_speed_hz = bus_speed;
        m_spi_interface_config.spics_io_num = cs; 
        m_spi_interface_config.queue_size = 5;

        spi_bus_add_device(m_spi_peripheral, &m_spi_interface_config, &m_spi_device_handle);

        return 0;
    }

    void SPI::setBusSpeed(const int bus_speed) {
        m_spi_interface_config.clock_speed_hz = bus_speed;
    }

    int SPI::getBusSpeed() {
        return m_spi_interface_config.clock_speed_hz;
    }

    spi_device_handle_t SPI::getSpiDeviceHandle() {
        return m_spi_device_handle;
    }

    uint8_t SPI::readRegister(const uint8_t reg_addr) {
        m_transferByte(reg_addr, 0, SPI_READ);

        return m_spi_transfer.rx_data[0];
    }

    int SPI::writeRegister(const uint8_t reg_addr, const uint8_t tx_data) {
        m_transferByte(reg_addr, tx_data, SPI_WRITE);

        return 0;
    }

    int SPI::writeRegisterMultipleBytes(const uint8_t reg_addr, uint8_t *tx_data, const int length) {
        return m_transferMultiplesBytes(reg_addr, tx_data, nullptr, length, SPI_WRITE);
    }

    int SPI::readRegisterMultipleBytes(const uint8_t reg_addr, uint8_t *rx_data, const int length) {   
        return m_transferMultiplesBytes(reg_addr, nullptr, rx_data, length, SPI_READ);
    }

    int SPI::read2Registers(const uint8_t reg) {
        uint8_t buff[2];
        readRegisterMultipleBytes(reg, buff, 2);
        return buff[1] << 8 | buff[0];
    }

    int SPI::initSpiForBme280(const int cs) {
        return registerDevice(MODE, cs, ADDR_BITS, CMD_BITS);
    }
} 



