#include "soc/gpio_periph.h"

#include <stdexcept>
#include <cstdio> // Dodanie nagłówka dla funkcji printf
namespace MZDK {
enum GpioMode {
    GPIO_MODE_DISABLE = 0,
    GPIO_MODE_INPUT = 1,
    GPIO_MODE_OUTPUT = 2,
    GPIO_MODE_OUTPUT_OD = 3, // Open Drain
    GPIO_MODE_INPUT_OUTPUT = 4,
    GPIO_MODE_INPUT_OUTPUT_OD = 5 // Open Drain with input
};

enum GpioPullMode {
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP = 1,
    GPIO_PULL_DOWN = 2
};

class GPIO {
private:
    int m_pin;
public:
    GPIO(int pin, GpioMode mode = GPIO_MODE_OUTPUT, GpioPullMode pull = GPIO_PULL_NONE);

    void setMode(GpioMode mode);
    void setPull(GpioPullMode pull);

    void setHigh();
    void setLow();

    bool read();
};
}
