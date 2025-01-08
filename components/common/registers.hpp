#pragma once

#include <cstdint>

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

// GPIO
#define GPIO_BASE 0x3FF44000
#define GPIO_OUT_REG (GPIO_BASE + 0x04)
#define GPIO_OUT_W1TS_REG (GPIO_BASE + 0x08)
#define GPIO_OUT_W1TC_REG (GPIO_BASE + 0x0C)
#define GPIO_ENABLE_REG (GPIO_BASE + 0x20)
#define GPIO_ENABLE_W1TS_REG (GPIO_BASE + 0x24)
#define GPIO_ENABLE_W1TC_REG (GPIO_BASE + 0x28)
#define GPIO_IN_REG (GPIO_BASE + 0x1C)
#define IO_MUX_GPIO21_REG 0x3FF4907C
#define IO_MUX_GPIO22_REG 0x3FF49080

// Definicje rejestrów dla I2C
#define I2C0_BASE 0x60013000
#define I2C1_BASE 0x60027000
#define I2C_FIFO_REG(base) ((base) + 0x0)
#define I2C_CTRL_REG(base) ((base) + 0xC)
#define I2C_CMD_REG(base) ((base) + 0x58)
#define I2C_STATUS_REG(base) ((base) + 0x60)
#define I2C_SCL_LOW_PERIOD_REG(base) ((base) + 0x1C)

// Definicje rejestrów dla SPI
#define SPI1_BASE 0x3FF64000
#define SPI2_BASE 0x3FF65000
#define SPI3_BASE 0x3FF66000
#define SPI_CMD_REG(base) ((base) + 0x00)
#define SPI_ADDR_REG(base) ((base) + 0x04)
#define SPI_CTRL_REG(base) ((base) + 0x08)
#define SPI_W0_REG(base) ((base) + 0x40)
#define SPI_STATUS_REG(base) ((base) + 0x18)

// Definicje rejestrów dla Timera Systemowego
#define SYS_TIMER_BASE 0x60026000
#define SYS_TIMER_COUNT_LOW (SYS_TIMER_BASE + 0x24)
#define SYS_TIMER_COUNT_HIGH (SYS_TIMER_BASE + 0x28)

