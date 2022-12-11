from machine import UART, I2C, Pin
from lib import mpr121
from encoding import electrode_uart_encoding
from init_functions import initialize_led_pins, initialize_buffer
import time


# Pins
TX, RX = 4, 5
SDA, SCL = 6, 7
LEDS = initialize_led_pins(range(10, 22))

# MPR121 constants
MPR_TOUCH_THRESHOLD = 10
MPR_RELEASE_THRESHOLD = 8

# Moving average filter
MA_BUFFER_SIZE = 50
MA_THRESHOLD = 0.1
buffer = initialize_buffer(MA_BUFFER_SIZE)

# UART communication
uart1 = UART(1, baudrate=9600, tx=Pin(TX), rx=Pin(RX), bits=8, parity=None, stop=1)

# I2C communication with MPR121
i2c = I2C(1, scl=Pin(SCL), sda=Pin(SDA), freq=100000)
time.sleep_ms(100)

mpr = mpr121.MPR121(i2c, 0x5A)
mpr.set_thresholds(MPR_TOUCH_THRESHOLD, MPR_RELEASE_THRESHOLD)


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
            LEDS[i].high()
            uart1.write(bytes(electrode_uart_encoding[i]['ON'], 'utf-8'))
        else:
            LEDS[i].low()
            uart1.write(bytes(electrode_uart_encoding[i]['OFF'], 'utf-8'))
