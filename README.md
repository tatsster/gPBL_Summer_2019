IOT project in gPBL with Shibaura Institute of Technology

## Introduction

This is IOT project for controlling room temperature with ESP32 board and mobile app.

Great thanks SIT assistant [Katsuya](https://github.com/Bo0km4n) in helping us deploy and config on ESP32 and ESP8266 boards.

TL,DR: Our final result of project is webapp_email/webapp_email.ino

## Contents 

- [Components](#components)
- [Install Hardware Info](#install-hardware-info)
- [Setup SMTP Server](#setup-smtp-server)
- [Code Explain](#code-explain)
- [Setup Mobile App](#setup-mobile-app)

## Components

In this project, we have 2 options for microcomputer:
- [ESPr One 32](https://www.switch-science.com/catalog/3555/)
- [ESPr One rev.3](https://www.switch-science.com/catalog/2620/)

These 2 boards are Japan-version of ESP32 and ESP8266 module which may be more familiar in other countries. Both share the same traits which can connect WiFi, bluetooth, HTTP GET & POST.

Another component is: [DHT11 Temperature Humidity Sensor](https://www.switch-science.com/catalog/4178/)

## Install Hardware Info

We develop most of our project in Arduino IDE and Visual Studio Code (with Arduino extension).

Because these are not by default Arduino board so we must install board information to deploy code from computer to devices.

### ESPr One 32

- Open Arduino IDE
- Open Preferences
- Add external board manager: "https://dl.espressif.com/dl/package_esp32_index.json"
- Reload Arduino IDE
- Open Tools => Board Manager
- Enter esp32 in search form
- Install **esp32 by Espressif Systems**
- Select Board manager **ESP32 Dev Module**

### ESPr One rev.3

- Open Arduino IDE
- Open Preferences
- Add external board manager: "http://arduino.esp8266.com/stable/package_esp8266com_index.json"
- Reload Arduino IDE
- Open Tools => Board Manager
- Enter esp32 in search form
- Install **esp8266 by ESP8266 Community**
- Select Board manager **Generic ESP8266 Module**