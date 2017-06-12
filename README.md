# MCU Code

This repository contains the microcontroller code for the webcam project detailed fully in [this repository](https://github.com/nathanshelly/webcam). Please read the introduction there before investigating this code. This code is designed to run on an Atmel SAM4S8B microcontroller.

Our code design includes a fairly high-level main function, which calls code in a number of somewhat modular helper files to communicate with the camera, microphone, and wifi chip.

Once each module is configured, the control alternates between two states: image capture and image send. In both states, audio streams from its circular buffer whenever possible, so as to maintain smooth audio for the listener. Specifically, during image sending, audio packets are prioritized over image packets - no image packet can send until audio is completely caught up to capture.  
