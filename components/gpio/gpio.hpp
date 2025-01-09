#pragma once

#include "soc/gpio_periph.h"
#include <cstdio> // Added header for printf function

namespace MZDK {

/**
 * @brief Enumeration representing GPIO modes.
 */
enum GpioMode {
    GPIO_MODE_DISABLE = 0,       ///< GPIO disabled.
    GPIO_MODE_INPUT = 1,         ///< GPIO set as input.
    GPIO_MODE_OUTPUT = 2,        ///< GPIO set as output.
    GPIO_MODE_OUTPUT_OD = 3,     ///< GPIO set as open-drain output.
    GPIO_MODE_INPUT_OUTPUT = 4,  ///< GPIO set as input and output.
    GPIO_MODE_INPUT_OUTPUT_OD = 5 ///< GPIO set as open-drain input and output.
};

/**
 * @brief Enumeration representing GPIO pull modes.
 */
enum GpioPullMode {
    GPIO_PULL_NONE = 0, ///< No pull resistor.
    GPIO_PULL_UP = 1,   ///< Pull-up resistor enabled.
    GPIO_PULL_DOWN = 2  ///< Pull-down resistor enabled.
};

/**
 * @brief Class representing a GPIO pin and its configuration.
 */
class GPIO {
private:
    int m_pin; ///< GPIO pin number.

public:
    /**
     * @brief Constructs a GPIO object with the specified configuration.
     * 
     * @param pin GPIO pin number.
     * @param mode Initial GPIO mode.
     * @param pull Initial GPIO pull mode.
     */
    GPIO(uint16_t pin, GpioMode mode, GpioPullMode pull);

    /**
     * @brief Sets the GPIO mode.
     * 
     * @param mode New GPIO mode.
     */
    void setMode(GpioMode mode);

    /**
     * @brief Sets the GPIO pull mode.
     * 
     * @param pull New GPIO pull mode.
     */
    void setPull(GpioPullMode pull);

    /**
     * @brief Sets the GPIO pin to high level.
     */
    void setHigh();

    /**
     * @brief Sets the GPIO pin to low level.
     */
    void setLow();
};

} // namespace MZDK

