# WeatherStation

Welcome to the WeatherStation project! This repository contains code to build a weather station using various sensors and microcontrollers.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Hardware Requirements](#hardware-requirements)
- [Configuration](#configuration)

## Introduction

The WeatherStation project aims to provide a comprehensive and easy-to-use solution for building a personal weather station. The project supports various sensors to measure temperature, humidity, pressure, and more.

## Features

- Measure temperature, humidity, and pressure.
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

- Microcontroller ESP32
- Sensor BME280
- Breadboard and jumper wires
- Power supply (e.g., USB cable, batteries)

## Configuration

To configure the WeatherStation project, you may need to modify certain files and settings:

- config.h: Contains configuration settings for the sensors and microcontroller.
- main.cpp: Main application code where you can customize the sensor readings and data logging.
