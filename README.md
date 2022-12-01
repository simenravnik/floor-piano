# Floor piano

## Installation instructions

Plug `Raspberry Pi Pico` into you computer while holding (!) the BOOTSEL button and release the button after a few seconds. Rasberry will appear as a USB storage device. This is controlled by the pico's boot program. Once you upload an application program, the boot program transfers control to it, and the pico disappears as a USB storage device - as it's designed to.

# Slave

`Raspberry Pi Pico` is used for reading the touch sensor data from the `MPR121` device which is connected to the keys on the piano. MPR121 library (https://github.com/BareConductive/mpr121) is used for communicating with the sensor.

# Master

TODO