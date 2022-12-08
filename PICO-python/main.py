from machine import UART, I2C, Pin
from lib import mpr121
from encoding import electrode_uart_encoding
import time

# UART communication
uart1 = UART(1, baudrate=9600, tx=Pin(4), rx=Pin(5), bits=8, parity=None, stop=1)

# I2C communication with MPR121
i2c = I2C(1, scl=Pin(7), sda=Pin(6), freq=100000)
time.sleep_ms(100)

mpr = mpr121.MPR121(i2c, 0x5A)
mpr.set_thresholds(10, 8)

last = 0

# MPR121 electrodes to Pico Pins mapping
# Electrode: Pico Pin
leds = dict()
# Set Pins from 10 to 22 as output pins
electrode = 0
for i in range(10, 22):
    led = Pin(i, Pin.OUT)
    leds[electrode] = led
    electrode += 1


def check(pin):
    global uart1
    global last
    touched = mpr.touched()
    diff = last ^ touched
    for i in range(12):
        if diff & (1 << i):
            if touched & (1 << i):
                print('Key {} pressed'.format(i))
                leds[i].high()
                uart1.write(bytes(electrode_uart_encoding[i]['ON'], 'utf-8'))
            else:
                print('Key {} released'.format(i))
                leds[i].low()
                uart1.write(bytes(electrode_uart_encoding[i]['OFF'], 'utf-8'))
    last = touched


i = Pin(8, Pin.IN, Pin.PULL_UP)
i.irq(check, Pin.IRQ_FALLING)
