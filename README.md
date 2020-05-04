# ESP32 BeeBot clone


## Introduction

Welcome to ESP32 Lienka (beebot clone) repository!

This project adds new remote functionality to lienka robot.
There are 2 main parts of this project:

- web application, which is run on your machine (not rendered from esp32)
- preconfigured esp32 project

### Configure the project

For making changes in config, please follow
```
make menuconfig
```
#### What can be customized in config?
-  (in the main menu) Wifi configuration where you can edit the robot ssid and password when initially in AP mode
-  (Component config -> Power Management) one can get faster response from lienka server when turning off power management (or just turning off DFS option)
-  (Component config -> Camera Configuration) if you use different camera for QR recognition (OV7670 by default)


### Web application

Web application is not part of the robot itself but is run from your machine. The simplest way to run it is (if you have python preinstalled)
```
python -m http.server --bind 0.0.0.0 80
```
inside the html directory in terminal. It is necessary to run server and not to only open html files from their location because the web app uses `Cross-Origin Resource Sharing` that is not allowed in some browsers when just opening single html file.

Web application consists of 4 pages
1. index.html - where user can edit mdns name, set your wlan network information for STA mode, reset the whole settings to default and check for mdns name and status of device (whether it is in STA(station) or AP(access-point) mode)
2. controls.html - basic control of lienka, after pushing green button, instructions are send to device and variable which stores instructions is cleared
3. controlsboard.html - tileboard that contains tiles for going forward and backward, for 90 degree turn left and right and also for making simple loops. Loops are set by first adding the number (2 or 3 dot purple circle), then green start tile (analogy to left round bracket) and after directions red stop tile (analogy to right round bracket). When ready to send it to the robot, just hit the orange button.
4. qr-recognition.html - Simple layout where by hitting "Look" button robot activates its camera and analyze the image for QR code. If QR code is valid, the "run" button is enabled and user can by pressing it send movement instruction to the robot.

Most js manipulation with layout is done by JQuery (events,ajax etc...). There is also parser.js file that contains Parser "class" (class is just syntax sugar for easier manipulation with the prototype) and web app uses its Syntax Analyzer when sending instructions to make sure the code is valid. This class also contains simple interpreting but this functionality is not used because the interpretation is currently done by the robot.


### ESP32 project

This project is developed using [ESP-IDF](https://github.com/espressif/esp-idf). Functionality is divided into the following files

#### beebot_main.c

This file contains the **app_main()** function that initializes other parts of the robot, namely
1. NVS (non-volatile storage) which is used for storing information like user's network ssid and password or mdns name
2. AP/STA setup, depending on if user already set network for STA mode
3. mDNS service (multicast DNS) that can be changed in the web app, default is **lienka**
4. http server
5. stepper init - to set proper mode for GPIO pins attached to the stepper motors
6. camera init - initialization of component copied from [ESP32_CAMERA_QR](https://github.com/jjsch-dev/ESP32_CAMERA_QR.git) repository

#### setup_ap.c and setup_sta.c

These files define initialization of wifi modes access-point and station. If STA ssid and password are set but robot is unable to find it, it is automatically switched to the AP.

#### setup_server.c

This file contains http server initialization and handlers for all registered URI.

#### stepper.c

Contains definition of FULL_STEP and HALF_STEP sequencies (currently using FULL_STEP). There is also writeSequence method which makes the actual stepper movement and calculations for number of sequences for specified length or angle to move depending on wheel radius and distance between wheels.

#### translator.c

This class contains function for translation from the Web app code to the Random access machine like instructions (MOV,JMP,JZ ...).

#### perform.c

The actual run of translated code is performed by this class that simulates run of random access machine.

#### qr_recognition.c

This file defines functionality for QR recognition inspired by [ESP32_CAMERA_QR](https://github.com/jjsch-dev/ESP32_CAMERA_QR.git) demo app.


#### other_functionality.c

Stores functions used in other parts of the code but these are not directly related to the functionaity in other files.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py build flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

