from machine import Pin


def initialize_led_pins(pins):
    # MPR121 electrodes to Pico Pins mapping
    # Electrode: Pico Pin
    leds = dict()
    # Set Pins inside "pins" as outputs
    electrode = 0
    for i in pins:
        led = Pin(i, Pin.OUT)
        leds[electrode] = led
        electrode += 1
    return leds


def initialize_buffer(buffer_size):
    # Initialize buffer of zeros for each electrode
    buffer = dict()
    for i in range(12):
        buffer[i] = [0] * buffer_size
    return buffer
