from machine import I2C, Pin
from lib import mpr121
import time

i2c = I2C(0, scl=Pin(5), sda=Pin(4), freq=100000)
time.sleep_ms(100)

mpr = mpr121.MPR121(i2c, 0x5A)
mpr.set_thresholds(30, 25)

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
    global last
    touched = mpr.touched()
    diff = last ^ touched
    for i in range(12):
        if diff & (1 << i):
            if touched & (1 << i):
                print('Key {} pressed'.format(i))
                leds[i].high()
            else:
                print('Key {} released'.format(i))
                leds[i].low()
    last = touched


i = Pin(3, Pin.IN, Pin.PULL_UP)
i.irq(check, Pin.IRQ_FALLING)
