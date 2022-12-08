
# UART is byte based protocol, and to avoid sending
# multiple bytes we encode the message the following:
# ON -> electrode pressed
# OFF -> electrode released
electrode_uart_encoding = {
    0: {"ON": "A", "OFF": "a"},
    1: {"ON": "B", "OFF": "b"},
    2: {"ON": "C", "OFF": "c"},
    3: {"ON": "D", "OFF": "d"},
    4: {"ON": "E", "OFF": "e"},
    5: {"ON": "F", "OFF": "f"},
    6: {"ON": "G", "OFF": "g"},
    7: {"ON": "H", "OFF": "h"},
    8: {"ON": "I", "OFF": "i"},
    9: {"ON": "J", "OFF": "j"},
    10: {"ON": "K", "OFF": "k"},
    11: {"ON": "L", "OFF": "l"},
}
