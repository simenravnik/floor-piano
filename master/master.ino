#include "src/VS10XX.h"

// VS1003 Module pin definitions
#define VS_XCS 8    // Control Chip Select Pin (for accessing SPI Control/Status registers)
#define VS_XDCS 9   // Data Chip Select / BSYNC Pin
#define VS_DREQ 7   // Data Request Pin: Player asks for more data
#define VS_RESET 10 // Reset is active low
// VS10xx SPI pin connections
// Provided here for info only - not used in the sketch as the SPI library handles this
#define VS_MOSI 11
#define VS_MISO 12
#define VS_SCK 13
#define VS_SS 10

VS10XX vs10xx(VS_XCS, VS_XDCS, VS_DREQ, VS_RESET, VS_SS);

// Use binary to say which MIDI channels this should respond to.
// Every "1" here enables that channel. Set all bits for all channels.
// Make sure the bit for channel 10 is set if you want drums.
//
//                               16  12  8   4  1
//                               |   |   |   |  |
uint16_t MIDI_CHANNEL_FILTER = 0b1111111111111111;

// List of instruments to send to any configured MIDI channels.
// Can use any GM MIDI voice numbers here (1 to 128), or more specific definitions
// (for example as found in vs1003inst.h for the VS1003).
//
// 0 means "ignore"
//
byte preset_instruments[16] = {
    /* 01 */ 1,
    /* 02 */ 9,
    /* 03 */ 17,
    /* 04 */ 25,
    /* 05 */ 30,
    /* 06 */ 33,
    /* 07 */ 41,
    /* 08 */ 49,
    /* 09 */ 57,
    /* 10 */ 0, // Channel 10 will be ignored later as that is percussion anyway.
    /* 11 */ 65,
    /* 12 */ 73,
    /* 13 */ 81,
    /* 14 */ 89,
    /* 15 */ 113,
    /* 16 */ 48};

byte instrument;
byte volume;

void setup()
{
    Serial.begin(9600);
    Serial.println("Initialising VS10xx");

    // put your setup code here, to run once:
    vs10xx.initialiseVS10xx();
    delay(1000);

    vs10xx.initVS10xxChannels(preset_instruments, MIDI_CHANNEL_FILTER);

    // Set these invalid to trigger a read of the pots
    // (if being used) first time through.
    instrument = -1;
    volume = -1;
}

#define C 0
#define C_sharp 1
#define D 2
#define D_sharp 3
#define E 4
#define F 5
#define F_sharp 6
#define G 7
#define G_sharp 8
#define A 9
#define A_sharp 10
#define B 11

// #define number_of_notes 14
#define number_of_notes 62
uint8_t christmas_song[number_of_notes] = {
    G, B, D, F_sharp, G, F_sharp, E, D,       // 8
    A, G, G, F_sharp, G, F_sharp, E, D,       // 8
    C, E, G, A, A_sharp, A, G, E,             // 8
    C, D_sharp, G, A, A_sharp, A, F, D_sharp, // 8

    G, A, F_sharp, G, E, F_sharp, E, D_sharp, // 8
    B, A, G, F_sharp, E, F_sharp, E, D_sharp, // 8
    D, E, G, D, C,                            // 5
    B, A, G, E, D_sharp, A, B, A, G           // 9
};

uint8_t octaves[number_of_notes] = {
    36, 36, 48, 48, 48, 48, 48, 48,
    48, 48, 48, 48, 48, 48, 48, 48,
    48, 48, 48, 48, 48, 48, 48, 48,
    48, 48, 48, 48, 48, 48, 48, 48,

    48, 48, 48, 48, 48, 48, 48, 48,
    48, 48, 48, 48, 48, 48, 48, 48,
    48, 48, 48, 60, 60,
    48, 48, 48, 48, 48, 48, 48, 48, 48};

uint8_t note_duration[number_of_notes] = {
    4, 4, 4, 1, 4, 4, 4, 4,
    4, 4, 4, 1, 4, 4, 4, 4,
    4, 4, 4, 1, 4, 4, 4, 4,
    4, 4, 4, 1, 4, 4, 4, 4,

    4, 4, 4, 1, 4, 4, 4, 4,
    4, 4, 4, 1, 4, 4, 4, 4,
    4, 4, 4, 1, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4};

uint8_t space_duration[number_of_notes] = {
    4, 4, 4, 4, 4, 4, 4, 8,
    4, 4, 4, 4, 4, 4, 4, 8,
    4, 4, 4, 4, 4, 4, 4, 8,
    4, 4, 4, 4, 4, 4, 4, 8,

    4, 4, 4, 4, 4, 4, 4, 8,
    4, 4, 4, 4, 4, 4, 4, 8,
    4, 4, 4, 4, 32,
    4, 4, 4, 4, 4, 12, 4, 4, 50};

void loop()
{

    // instrument = 74;
    // vs10xx.talkMIDI(0xC0, instrument, 0);

    // vs10xx.talkMIDI(0xB0, 0x00, 120); // 0xB0 is channel message, set channel volume to near max (127)
    // vs10xx.talkMIDI(0xB0, 0, 0x00);

    for (int i = 0; i < number_of_notes; i++)
    {
        uint8_t note = christmas_song[i] + octaves[i];
        uint8_t note2 = christmas_song[i] + octaves[i] + 12;

        // Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
        vs10xx.noteOn(0, note, 127);
        vs10xx.noteOn(0, note2, 127);
        delay(note_duration[i] * 50);

        // Turn off the note with a given off/release velocity
        vs10xx.noteOff(0, note, 127);
        vs10xx.noteOff(0, note2, 127);
        delay(space_duration[i] * 50);
    }
}