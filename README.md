# WeatherStation

Welcome to the WeatherStation project! This repository contains code to build a weather station using various sensors and microcontrollers. The main language used is C++, with some portions in C and CMake for build management.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Hardware Requirements](#hardware-requirements)
- [Configuration](#configuration)
- [Testing](#testing)
- [Contributing](#contributing)
- [License](#license)

## Introduction

The WeatherStation project aims to provide a comprehensive and easy-to-use solution for building a personal weather station. The project supports various sensors to measure temperature, humidity, pressure, and more.

## Features

- Measure temperature, humidity, and pressure.
- Supports multiple sensor types.
- Data logging capabilities.
- Easy to set up and configure.
- Real-time data monitoring.
- Modular and extensible design.

## Installation

To install the WeatherStation project, follow these steps:

1. Clone the repository:
    ```sh
    git clone https://github.com/w1nn3t0u-o7/WeatherStation.git
    ```
2. Navigate to the project directory:
    ```sh
    cd WeatherStation
    ```
3. Build the project using CMake:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

## Usage

Once the project is built, you can run the weather station software using the following command:
```sh
./WeatherStation
```

## Hardware Requirements

    - Microcontroller (e.g., Arduino, ESP8266, ESP32)
    - Temperature sensor (e.g., DHT22, DS18B20)
    - Humidity sensor (e.g., DHT22)
    - Pressure sensor (e.g., BMP180, BMP280)
    - Breadboard and jumper wires
    - Power supply (e.g., USB cable, batteries)

## Configuration

To configure the WeatherStation project, you may need to modify certain files and settings:

    - config.h: Contains configuration settings for the sensors and microcontroller.
    - main.cpp: Main application code where you can customize the sensor readings and data logging.

## Testing

To test the WeatherStation project, you can use the following methods:

    - Unit tests: Run the unit tests using the testing framework included in the project.
    - Manual testing: Connect the sensors to the microcontroller and verify the readings on the serial monitor or display.

## Contributing

We welcome contributions to the WeatherStation project! To contribute, please fork the repository, create a new branch for your changes, and submit a pull request. Ensure your code adheres to the project's coding standards and includes appropriate tests.
## License

This project is licensed under the MIT License. See the LICENSE file for more details.
