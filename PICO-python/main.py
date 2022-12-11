from machine import UART, I2C, Pin
from lib import mpr121
from encoding import electrode_uart_encoding
import time

# Constants
BUFFER_SIZE = 50
MA_THRESHOLD = 0.1  # Moving average threshold

# UART communication
uart1 = UART(1, baudrate=9600, tx=Pin(4), rx=Pin(5), bits=8, parity=None, stop=1)

# I2C communication with MPR121
i2c = I2C(1, scl=Pin(7), sda=Pin(6), freq=100000)
time.sleep_ms(100)

mpr = mpr121.MPR121(i2c, 0x5A)
mpr.set_thresholds(10, 8)

# Initialize buffer of zeros for each electrode
buffer = dict()
for i in range(12):
    buffer[i] = [0] * BUFFER_SIZE

# MPR121 electrodes to Pico Pins mapping
# Electrode: Pico Pin
leds = dict()
# Set Pins from 10 to 22 as output pins
electrode = 0
for i in range(10, 22):
    led = Pin(i, Pin.OUT)
    leds[electrode] = led
    electrode += 1


while True:
    touched = mpr.touched()
    for i in range(12):

        # Update buffer values
        if touched & (1 << i):
            print("Buffer length = ", len(buffer[i]))
            buffer[i].insert(0, 1)  # Insert 1 to the index 0
            buffer[i].pop()
        else:
            buffer[i].insert(0, 0)  # Insert 0 to the index 0
            buffer[i].pop()

        # Average of the i-th electrode buffer
        avg = sum(buffer[i]) / len(buffer[i])

        # Update the led/sound state if needed
        # TODO: Store previous state and update only when triggered
        if avg > MA_THRESHOLD:
            leds[i].high()
            uart1.write(bytes(electrode_uart_encoding[i]['ON'], 'utf-8'))
        else:
            leds[i].low()
            uart1.write(bytes(electrode_uart_encoding[i]['OFF'], 'utf-8'))
