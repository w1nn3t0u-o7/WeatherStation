#include "gpio.hpp"

MZDK::GPIO::GPIO(uint16_t pin, GpioMode mode, GpioPullMode pull) : m_pin(pin) {
    setMode(mode);
    setPull(pull);
}

void MZDK::GPIO::setMode(GpioMode mode) {

    switch (mode) {
        case GPIO_MODE_DISABLE:
            REG_WRITE(GPIO_ENABLE_REG, REG_READ(GPIO_ENABLE_REG) & ~BIT(m_pin));
            break;
        case GPIO_MODE_INPUT:
            REG_WRITE(GPIO_ENABLE_REG, REG_READ(GPIO_ENABLE_REG) & ~BIT(m_pin));
            break;
        case GPIO_MODE_OUTPUT:
            REG_WRITE(GPIO_ENABLE_REG, REG_READ(GPIO_ENABLE_REG) | BIT(m_pin));
            REG_WRITE(GPIO_REG(m_pin), REG_READ(GPIO_REG(m_pin)) & ~GPIO_PIN_PAD_DRIVER);
            break;
        case GPIO_MODE_OUTPUT_OD:
            REG_WRITE(GPIO_ENABLE_REG, REG_READ(GPIO_ENABLE_REG) | BIT(m_pin));
            REG_WRITE(GPIO_REG(m_pin), REG_READ(GPIO_REG(m_pin)) | GPIO_PIN_PAD_DRIVER);
            break;
        case GPIO_MODE_INPUT_OUTPUT:
            REG_WRITE(GPIO_ENABLE_REG, REG_READ(GPIO_ENABLE_REG) | BIT(m_pin));
            REG_WRITE(GPIO_REG(m_pin), REG_READ(GPIO_REG(m_pin)) & ~GPIO_PIN_PAD_DRIVER);
            break;
        case GPIO_MODE_INPUT_OUTPUT_OD:
            REG_WRITE(GPIO_ENABLE_REG, REG_READ(GPIO_ENABLE_REG) | BIT(m_pin));
            REG_WRITE(GPIO_REG(m_pin), REG_READ(GPIO_REG(m_pin)) | GPIO_PIN_PAD_DRIVER);
            break;
    }
}

void MZDK::GPIO::setPull(GpioPullMode pull) {

    switch (pull) {
        case GPIO_PULL_UP:
            REG_SET_BIT(GPIO_PIN_MUX_REG[m_pin], FUN_PU);
            REG_CLR_BIT(GPIO_PIN_MUX_REG[m_pin], FUN_PD);
            break;
        case GPIO_PULL_DOWN:
            REG_CLR_BIT(GPIO_PIN_MUX_REG[m_pin], FUN_PU);
            REG_SET_BIT(GPIO_PIN_MUX_REG[m_pin], FUN_PD);
            break;
        case GPIO_PULL_NONE:
            REG_CLR_BIT(GPIO_PIN_MUX_REG[m_pin], FUN_PU);
            REG_CLR_BIT(GPIO_PIN_MUX_REG[m_pin], FUN_PD);
            break;
    }
}

void MZDK::GPIO::setHigh() {
    REG_WRITE(GPIO_OUT_W1TS_REG, BIT(m_pin));
}

void MZDK::GPIO::setLow() {
    REG_WRITE(GPIO_OUT_W1TC_REG, BIT(m_pin));
}

bool MZDK::GPIO::read() {
    return (REG_READ(GPIO_IN_REG) & BIT(m_pin)) != 0;
}
