#include "soc/gpio_periph.h"

#include <stdexcept>
#include <cstdio> // Dodanie nagłówka dla funkcji printf
namespace MZDK {
typedef enum {
    GPIO_MODE_DISABLE = 0,
    GPIO_MODE_INPUT = 1,
    GPIO_MODE_OUTPUT = 2,
    GPIO_MODE_OUTPUT_OD = 3, // Open Drain
    GPIO_MODE_INPUT_OUTPUT = 4,
    GPIO_MODE_INPUT_OUTPUT_OD = 5 // Open Drain with input
} gpio_mode_t;

typedef enum {
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP = 1,
    GPIO_PULL_DOWN = 2
} gpio_pull_t;

class GPIO {
public:
    GPIO(int pin, gpio_mode_t mode = GPIO_MODE_OUTPUT, gpio_pull_t pull = GPIO_PULL_NONE)
        : pin_(pin) {
        if (pin < 0 || pin > 39) {
            throw std::invalid_argument("Invalid GPIO pin number");
        }
        setMode(mode);
        setPull(pull);
    }

    void setMode(gpio_mode_t mode) {
        if (mode < GPIO_MODE_DISABLE || mode > GPIO_MODE_INPUT_OUTPUT_OD) {
            throw std::invalid_argument("Invalid GPIO mode");
        }

        switch (mode) {
            case GPIO_MODE_DISABLE:
                REG_WRITE(GPIO_ENABLE_REG, REG_READ(GPIO_ENABLE_REG) & ~BIT(pin_));
                break;
            case GPIO_MODE_INPUT:
                REG_WRITE(GPIO_ENABLE_REG, REG_READ(GPIO_ENABLE_REG) & ~(1 << pin_));
                break;
            case GPIO_MODE_OUTPUT:
                REG_WRITE(GPIO_ENABLE_REG, REG_READ(GPIO_ENABLE_REG) | (1 << pin_));
                REG_WRITE(GPIO_REG(pin_), REG_READ(GPIO_REG(pin_)) & ~GPIO_PIN_PAD_DRIVER);
                break;
            case GPIO_MODE_OUTPUT_OD:
                REG_WRITE(GPIO_ENABLE_REG, REG_READ(GPIO_ENABLE_REG) | (1 << pin_));
                REG_WRITE(GPIO_REG(pin_), REG_READ(GPIO_REG(pin_)) | GPIO_PIN_PAD_DRIVER);
                break;
            case GPIO_MODE_INPUT_OUTPUT:
                REG_WRITE(GPIO_ENABLE_REG, REG_READ(GPIO_ENABLE_REG) | (1 << pin_));
                REG_WRITE(GPIO_REG(pin_), REG_READ(GPIO_REG(pin_)) & ~GPIO_PIN_PAD_DRIVER);
                break;
            case GPIO_MODE_INPUT_OUTPUT_OD:
                REG_WRITE(GPIO_ENABLE_REG, REG_READ(GPIO_ENABLE_REG) | (1 << pin_));
                REG_WRITE(GPIO_REG(pin_), REG_READ(GPIO_REG(pin_)) | GPIO_PIN_PAD_DRIVER);
                break;
        }
    }

    void setPull(gpio_pull_t pull) {
        if (pull < GPIO_PULL_NONE || pull > GPIO_PULL_DOWN) {
            throw std::invalid_argument("Invalid GPIO pull configuration");
        }

        switch (pull) {
            case GPIO_PULL_UP:
                REG_SET_BIT(GPIO_PIN_MUX_REG[pin_], FUN_PU);
                REG_CLR_BIT(GPIO_PIN_MUX_REG[pin_], FUN_PD);
                break;
            case GPIO_PULL_DOWN:
                REG_CLR_BIT(GPIO_PIN_MUX_REG[pin_], FUN_PU);
                REG_SET_BIT(GPIO_PIN_MUX_REG[pin_], FUN_PD);
                break;
            case GPIO_PULL_NONE:
                REG_CLR_BIT(GPIO_PIN_MUX_REG[pin_], FUN_PU);
                REG_CLR_BIT(GPIO_PIN_MUX_REG[pin_], FUN_PD);
                break;
        }
    }

    void setHigh() {
        REG_WRITE(GPIO_OUT_W1TS_REG, 1 << pin_);
    }

    void setLow() {
        REG_WRITE(GPIO_OUT_W1TC_REG, 1 << pin_);
    }

    bool read() {
        return (REG_READ(GPIO_IN_REG) & (1 << pin_)) != 0;
    }

private:
    int pin_;
};
}
