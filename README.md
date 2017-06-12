# MCU Code

## Description

This repository contains the microcontroller code for the webcam project detailed fully in [this repository](https://github.com/nathanshelly/webcam). Please read the introduction there before investigating this code. This code is designed to run on an Atmel SAM4S8B microcontroller.

Our code design includes a fairly high-level main function, which calls code in a number of somewhat modular helper files to communicate with the camera, microphone, and wifi chip.

Once each module is configured, the control alternates between two states: image capture and image send. In both states, audio streams from its circular buffer whenever possible, so as to maintain smooth audio for the listener. Specifically, during image sending, audio packets are prioritized over image packets - no image packet can send until audio is completely caught up to capture.

## Changes before running

Assuming you've followed the steps from our tutorial in the other repository the only change you should need to make to this code is in the open_websocket function. The `wss://bigbrothersees.me/source_socket` in the following line: 

```
uint8_t status_code = write_wifi_command_safe("websocket_client -f bin wss://bigbrothersees.me/source_socket\r\n", "Opened: ", 20000, 1);
```

should be changed to whatever websocket URL you'd like to receive image and audio data at.