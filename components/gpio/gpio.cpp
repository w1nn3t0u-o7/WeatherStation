#include "gpio.hpp"

/**
 * @brief Constructs a GPIO object and configures it with the specified mode and pull settings.
 * 
 * @param pin GPIO pin number.
 * @param mode Initial GPIO mode.
 * @param pull Initial GPIO pull mode.
 */
MZDK::GPIO::GPIO(uint16_t pin, GpioMode mode, GpioPullMode pull) : m_pin(pin) {
    setMode(mode);
    setPull(pull);
}

/**
 * @brief Sets the GPIO mode for the pin.
 * 
 * @param mode New GPIO mode to configure.
 */
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

/**
 * @brief Sets the GPIO pull mode for the pin.
 * 
 * @param pull New GPIO pull mode to configure.
 */
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

/**
 * @brief Sets the GPIO pin to a high logic level.
 */
void MZDK::GPIO::setHigh() {
    REG_WRITE(GPIO_OUT_W1TS_REG, BIT(m_pin));
}

/**
 * @brief Sets the GPIO pin to a low logic level.
 */
void MZDK::GPIO::setLow() {
    REG_WRITE(GPIO_OUT_W1TC_REG, BIT(m_pin));
}



