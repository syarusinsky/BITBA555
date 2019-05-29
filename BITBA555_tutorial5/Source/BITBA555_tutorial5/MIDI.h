#define MIDI_NOTE_OFF 0b1000
#define MIDI_NOTE_ON 0b1001
#define MIDI_EVENT_BUFFER_SIZE 10

// midi note values
#define MIDI_C0 0
#define MIDI_Db0 1
#define MIDI_D0 2
#define MIDI_Eb0 3
#define MIDI_E0 4
#define MIDI_F0 5
#define MIDI_Gb0 6
#define MIDI_G0 7
#define MIDI_Ab0 8
#define MIDI_A0 9
#define MIDI_Bb0 10
#define MIDI_B0 11
#define MIDI_C1 12
#define MIDI_Db1 13
#define MIDI_D1 14
#define MIDI_Eb1 15
#define MIDI_E1 16
#define MIDI_F1 17
#define MIDI_Gb1 18
#define MIDI_G1 19
#define MIDI_Ab1 20
#define MIDI_A1 21
#define MIDI_Bb1 22
#define MIDI_B1 23
#define MIDI_C2 24
#define MIDI_Db2 25
#define MIDI_D2 26
#define MIDI_Eb2 27
#define MIDI_E2 28
#define MIDI_F2 29
#define MIDI_Gb2 30
#define MIDI_G2 31
#define MIDI_Ab2 32
#define MIDI_A2 33
#define MIDI_Bb2 34
#define MIDI_B2 35
#define MIDI_C3 36
#define MIDI_Db3 37
#define MIDI_D3 38
#define MIDI_Eb3 39
#define MIDI_E3 40
#define MIDI_F3 41
#define MIDI_Gb3 42
#define MIDI_G3 43
#define MIDI_Ab3 44
#define MIDI_A3 45
#define MIDI_Bb3 46
#define MIDI_B3 47
#define MIDI_C4 48
#define MIDI_Db4 49
#define MIDI_D4 50
#define MIDI_Eb4 51
#define MIDI_E4 52
#define MIDI_F4 53
#define MIDI_Gb4 54
#define MIDI_G4 55
#define MIDI_Ab4 56
#define MIDI_A4 57
#define MIDI_Bb4 58
#define MIDI_B4 59
#define MIDI_C5 60
#define MIDI_Db5 61
#define MIDI_D5 62
#define MIDI_Eb5 63
#define MIDI_E5 64
#define MIDI_F5 65
#define MIDI_Gb5 66
#define MIDI_G5 67
#define MIDI_Ab5 68
#define MIDI_A5 69
#define MIDI_Bb5 70
#define MIDI_B5 71
#define MIDI_C6 72
#define MIDI_Db6 73
#define MIDI_D6 74
#define MIDI_Eb6 75
#define MIDI_E6 76
#define MIDI_F6 77
#define MIDI_Gb6 78
#define MIDI_G6 79
#define MIDI_Ab6 80
#define MIDI_A6 81
#define MIDI_Bb6 82
#define MIDI_B6 83
#define MIDI_C7 84
#define MIDI_Db7 85
#define MIDI_D7 86
#define MIDI_Eb7 87
#define MIDI_E7 88
#define MIDI_F7 89
#define MIDI_Gb7 90
#define MIDI_G7 91
#define MIDI_Ab7 92
#define MIDI_A7 93
#define MIDI_Bb7 94
#define MIDI_B7 95
#define MIDI_C8 96
#define MIDI_Db8 97
#define MIDI_D8 98
#define MIDI_Eb8 99
#define MIDI_E8 100
#define MIDI_F8 101
#define MIDI_Gb8 102
#define MIDI_G8 103
#define MIDI_Ab8 104
#define MIDI_A8 105
#define MIDI_Bb8 106
#define MIDI_B8 107

volatile typedef struct midiEvent {  // Midi Event struct used for note on and note off messages
  uint8_t statusByte;
  uint8_t dataByte[2];
} MidiEvent;

MidiEvent midiEventBuffer[MIDI_EVENT_BUFFER_SIZE];  // a buffer to store MidiEvents for main loop to process
uint8_t midiReadIndex = 0;  // used by main loop to keep track of current read
volatile uint8_t midiWriteIndex = 0; // used by usart interrupt to keep track of current write, when midiReadIndex = midiWriteIndex then all data in buffer has been processed
volatile uint8_t midiWritingFlag = 255;  // if 255 then not currently writing data bytes, if 0 then writing dataByte 1, if 1 then writing dataByte 2

ISR (USART_RX_vect) // interrupt service routine called each time USART data is recieved
{
  uint8_t data = UDR0;  // receives data from USART data register
  uint8_t midiMessageType = (data >> 4);
  uint8_t isStatusByte = (data >> 7);

  if ( midiMessageType == MIDI_NOTE_ON || midiMessageType == MIDI_NOTE_OFF )  // if we recieve a midi note on message or a midi note off message, begin writing to the midi buffer
  {
    midiEventBuffer[midiWriteIndex].statusByte = data;  // write the status byte to the current midi write index
    midiWritingFlag = 0;  // set the writing flag to begin writing data bytes
  } else if ( isStatusByte == 0 && midiWritingFlag < 2 ) // we're only concerned with 2 data bytes, but if we were accepting messages of different lengths this value would have to be determined dynamically in the previous block
  {
    midiEventBuffer[midiWriteIndex].dataByte[midiWritingFlag] = data; // write data bytes to current midi write index

    if ( midiWritingFlag == 1 ) // if this is the last data byte for this note
    {
      uint8_t currentStatusByte = midiEventBuffer[midiWriteIndex].statusByte;
      midiWriteIndex = (midiWriteIndex + 1) % MIDI_EVENT_BUFFER_SIZE; // increment the midi write index since we've completed a midi event, use modulo operator for circular buffer
      
      midiEventBuffer[midiWriteIndex].statusByte = currentStatusByte;  // write the next midi event's status byte in case of running status
      midiWritingFlag = 0;  // set the writing flag to begin writing data bytes in case of running status 
    } else 
    {
      midiWritingFlag++;  // increment the midi writing flag in order to write the next data byte
    }
  } else // this means we've either received a different status byte than a note on or note off, or we've received a data byte that is not associated with a note on or note off message
  {
    midiWritingFlag = 255; // set the writing flag to be out of bounds, so that no more data bytes can be written until another note on or note off message is received
  }

  USART_Transmit (data);  // echo midi data
}
