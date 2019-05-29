#include "PeripheralSetup.h"
#include "SampleGeneration.h"
#include "MIDI.h"

#define INCREMENT_TABLE_SIZE 50   // an "increment table" is used to increment through the table at a given frequency without requiring floating point numbers (the larger the size, the more accurate the frequency)

#define GATE_OUT_PIN PINB4  // pinb4 will be used to control gate output and associated LED
#define WAVE_SELECTION_PIN PINB1  // pinb1 will be used as a button input to change waveforms

typedef struct voice  // this struct represents a synthesizer voice
{
  volatile uint16_t tablePos = 0;  // tablePos determines where in the waveform table the voice is currently
  uint8_t midiNote = 0; // midiNote records the current midi note value
  uint8_t ampVal = 0; // ampVal determines the amplitude value to apply to the note (should be 0 to 16)
  uint8_t incrTable[INCREMENT_TABLE_SIZE];  // the increment table values are used to increment through the table at a given frequency without requiring floating point numbers
  volatile uint8_t incrTablePos = 0;  // a value used to increment through the incrTable
  uint8_t previousNotes[10];  // these are the previous midiNote values in case of a key being released while other keys are still pressed, the Voice will play a previous note value until all keys are released
  uint8_t numNotesPressed = 0;  // the number of previous notes still being held
  uint8_t incrTableNeedsProc = 0; // 1 if the incrTable needs to be reprocessed, 0 if it doesn't
} Voice;

Voice osc; // the voice that will act as our digital oscillator

// Use this for tutorial before implementing MIDI notes
// volatile uint16_t timerWaveIncr = 0;

void processMidiEvent (const MidiEvent* const midiEvent); // takes in a MidiEvent and uses its data to correctly update the Voice data structure
void processIncrTable();  // processes the incrTable values for the Voice

uint8_t currentWaveIsSelected = 0;  // a boolean value used in the main loop to assure that while the button is being pressed, the synth isn't quickly flipping through waveforms
uint8_t currentWaveLocation = 0;  // this value is used to determine which wave in the waveBuffer is currently in use (0 for sine, 1 for triangle, 2 for square, 3 for sawtooth)

void setup () 
{
  cli();  // disable interrupts

  DDRB |= (1 << GATE_OUT_PIN);  // setup gate output/LED pin
  PORTB &= ~(1 << GATE_OUT_PIN); // set gate output/LED pin low
  DDRB &= ~(1 << WAVE_SELECTION_PIN); // set up wave selection pin as input

  // setup peripherals
  Timer1Initialize();
  USART_Init();
  SPIDAC_MasterInit();

  // setup waveform buffers
  generateSine();
  generateTriangle();
  generateSquare();
  generateSawtooth();
  
  sei();  // enable interrupts
}

void loop () 
{
  while (midiReadIndex != midiWriteIndex) // while there are MidiEvents in the MIDI event buffer, process them
  {
    processMidiEvent(&midiEventBuffer[midiReadIndex]);
    midiReadIndex = (midiReadIndex + 1) % MIDI_EVENT_BUFFER_SIZE;
  }

  if (osc.incrTableNeedsProc) // if incrTable needs processing (this is done after the processMidiEvent loop so that if a bunch of notes are pressed at once, we don't waste processing on all of them since their incrTables won't be used)
  {
    processIncrTable();
  }
  
  if (PINB & (1 << WAVE_SELECTION_PIN) && !currentWaveIsSelected) // if the wave selection button is pressed and it hasn't remained high since last loop...
  {
    currentWaveLocation = (currentWaveLocation + 1) % 4;
    switch (currentWaveLocation)
    {
      case 0  :
        currentWave = sineWave;
        break;
      case 1  :
        currentWave = triangleWave;
        break;
      case 2  :
        currentWave = squareWave;
        break;
      case 3  :
        currentWave = sawtoothWave;
        break;
      default :
        currentWave = sineWave;
    }
    currentWaveIsSelected = 1;
  } else if ( !(PINB & (1 << WAVE_SELECTION_PIN)) && currentWaveIsSelected )  // if the wave selection button isn't pressed and a wave was just selected...
  {
    currentWaveIsSelected = 0;
  }
}

void processMidiEvent (const MidiEvent* const midiEvent)
{
  if ( (midiEvent->statusByte >> 4) == MIDI_NOTE_OFF || midiEvent->dataByte[1] == 0) // if note off message,...
  {
    osc.numNotesPressed -= 1; // a key has been released so the number of notes pressed simultaneously is -= 1
    
    if (osc.numNotesPressed == 0) // if no more notes are being held
    {
      PORTB &= ~(1 << GATE_OUT_PIN);  // set gate output to low 
    } else
    {
      osc.midiNote = osc.previousNotes[osc.numNotesPressed];  // set the current note to the previous note in the buffer
      osc.incrTableNeedsProc = 1; // since we're using a previous note, the incrTable needs to be reprocessed
    }
  }
  else  // if note on message,... 
  {
    osc.midiNote = midiEvent->dataByte[0]; // set the midi note value of the Voice struct to the current midi note being processed
    osc.ampVal = round (midiEvent->dataByte[1] / 8);  // this will give us a amplitude of 0-16, which will keep us within clipping range (ex. 255 * 16 = 4080 < 4095)
    osc.numNotesPressed += 1; // this note is pressed so the number of notes pressed simultaneously is += 1
    osc.previousNotes[osc.numNotesPressed] = midiEvent->dataByte[0];  // put this value into the previous note buffer to be processed if a key after this is released but this key is still pressed
    osc.incrTableNeedsProc = 1; // we are processing a new note, so the incrTable needs to be reprocessed

    PORTB |= (1 << GATE_OUT_PIN); // set gate output to high to prepare outputting audio
  }
}

void processIncrTable()
{
  float runningIncr = 0.0;  // the running total for incrementing, to be rounded off and placed 
  float incr = 0.0; // the floating point value to increment by to achieve desired frequency
  
  switch (osc.midiNote) // matching the midi note value to the associated incr value
  {
    case MIDI_C0  :
      incr = MUSIC_C0;
      break;
    case MIDI_Db0  :
      incr = MUSIC_Db0;
      break;
    case MIDI_D0  :
      incr = MUSIC_D0;
      break;
    case MIDI_Eb0  :
      incr = MUSIC_Eb0;
      break;
    case MIDI_E0  :
      incr = MUSIC_E0;
      break;
    case MIDI_F0  :
      incr = MUSIC_F0;
      break;
    case MIDI_Gb0  :
      incr = MUSIC_Gb0;
      break;
    case MIDI_G0  :
      incr = MUSIC_G0;
      break;
    case MIDI_Ab0  :
      incr = MUSIC_Ab0;
      break;
    case MIDI_A0  :
      incr = MUSIC_A0;
      break;
    case MIDI_Bb0  :
      incr = MUSIC_Bb0;
      break;
    case MIDI_B0  :
      incr = MUSIC_B0;
      break;
    case MIDI_C1  :
      incr = MUSIC_C1;
      break;
    case MIDI_Db1  :
      incr = MUSIC_Db1;
      break;
    case MIDI_D1  :
      incr = MUSIC_D1;
      break;
    case MIDI_Eb1  :
      incr = MUSIC_Eb1;
      break;
    case MIDI_E1  :
      incr = MUSIC_E1;
      break;
    case MIDI_F1  :
      incr = MUSIC_F1;
      break;
    case MIDI_Gb1  :
      incr = MUSIC_Gb1;
      break;
    case MIDI_G1  :
      incr = MUSIC_G1;
      break;
    case MIDI_Ab1  :
      incr = MUSIC_Ab1;
      break;
    case MIDI_A1  :
      incr = MUSIC_A1;
      break;
    case MIDI_Bb1  :
      incr = MUSIC_Bb1;
      break;
    case MIDI_B1  :
      incr = MUSIC_B1;
      break;
    case MIDI_C2  :
      incr = MUSIC_C2;
      break;
    case MIDI_Db2  :
      incr = MUSIC_Db2;
      break;
    case MIDI_D2  :
      incr = MUSIC_D2;
      break;
    case MIDI_Eb2  :
      incr = MUSIC_Eb2;
      break;
    case MIDI_E2  :
      incr = MUSIC_E2;
      break;
    case MIDI_F2  :
      incr = MUSIC_F2;
      break;
    case MIDI_Gb2  :
      incr = MUSIC_Gb2;
      break;
    case MIDI_G2  :
      incr = MUSIC_G2;
      break;
    case MIDI_Ab2  :
      incr = MUSIC_Ab2;
      break;
    case MIDI_A2  :
      incr = MUSIC_A2;
      break;
    case MIDI_Bb2  :
      incr = MUSIC_Bb2;
      break;
    case MIDI_B2  :
      incr = MUSIC_B2;
      break;
    case MIDI_C3  :
      incr = MUSIC_C3;
      break;
    case MIDI_Db3  :
      incr = MUSIC_Db3;
      break;
    case MIDI_D3  :
      incr = MUSIC_D3;
      break;
    case MIDI_Eb3  :
      incr = MUSIC_Eb3;
      break;
    case MIDI_E3  :
      incr = MUSIC_E3;
      break;
    case MIDI_F3  :
      incr = MUSIC_F3;
      break;
    case MIDI_Gb3  :
      incr = MUSIC_Gb3;
      break;
    case MIDI_G3  :
      incr = MUSIC_G3;
      break;
    case MIDI_Ab3  :
      incr = MUSIC_Ab3;
      break;
    case MIDI_A3  :
      incr = MUSIC_A3;
      break;
    case MIDI_Bb3  :
      incr = MUSIC_Bb3;
      break;
    case MIDI_B3  :
      incr = MUSIC_B3;
      break;
    case MIDI_C4  :
      incr = MUSIC_C4;
      break;
    case MIDI_Db4  :
      incr = MUSIC_Db4;
      break;
    case MIDI_D4  :
      incr = MUSIC_D4;
      break;
    case MIDI_Eb4  :
      incr = MUSIC_Eb4;
      break;
    case MIDI_E4  :
      incr = MUSIC_E4;
      break;
    case MIDI_F4  :
      incr = MUSIC_F4;
      break;
    case MIDI_Gb4  :
      incr = MUSIC_Gb4;
      break;
    case MIDI_G4  :
      incr = MUSIC_G4;
      break;
    case MIDI_Ab4  :
      incr = MUSIC_Ab4;
      break;
    case MIDI_A4  :
      incr = MUSIC_A4;
      break;
    case MIDI_Bb4  :
      incr = MUSIC_Bb4;
      break;
    case MIDI_B4  :
      incr = MUSIC_B4;
      break;
    case MIDI_C5  :
      incr = MUSIC_C5;
      break;
    case MIDI_Db5  :
      incr = MUSIC_Db5;
      break;
    case MIDI_D5  :
      incr = MUSIC_D5;
      break;
    case MIDI_Eb5  :
      incr = MUSIC_Eb5;
      break;
    case MIDI_E5  :
      incr = MUSIC_E5;
      break;
    case MIDI_F5  :
      incr = MUSIC_F5;
      break;
    case MIDI_Gb5  :
      incr = MUSIC_Gb5;
      break;
    case MIDI_G5  :
      incr = MUSIC_G5;
      break;
    case MIDI_Ab5  :
      incr = MUSIC_Ab5;
      break;
    case MIDI_A5  :
      incr = MUSIC_A5;
      break;
    case MIDI_Bb5  :
      incr = MUSIC_Bb5;
      break;
    case MIDI_B5  :
      incr = MUSIC_B5;
      break;
    case MIDI_C6  :
      incr = MUSIC_C6;
      break;
    case MIDI_Db6  :
      incr = MUSIC_Db6;
      break;
    case MIDI_D6  :
      incr = MUSIC_D6;
      break;
    case MIDI_Eb6  :
      incr = MUSIC_Eb6;
      break;
    case MIDI_E6  :
      incr = MUSIC_E6;
      break;
    case MIDI_F6  :
      incr = MUSIC_F6;
      break;
    case MIDI_Gb6  :
      incr = MUSIC_Gb6;
      break;
    case MIDI_G6  :
      incr = MUSIC_G6;
      break;
    case MIDI_Ab6  :
      incr = MUSIC_Ab6;
      break;
    case MIDI_A6  :
      incr = MUSIC_A6;
      break;
    case MIDI_Bb6  :
      incr = MUSIC_Bb6;
      break;
    case MIDI_B6  :
      incr = MUSIC_B6;
      break;
    case MIDI_C7  :
      incr = MUSIC_C7;
      break;
    case MIDI_Db7  :
      incr = MUSIC_Db7;
      break;
    case MIDI_D7  :
      incr = MUSIC_D7;
      break;
    case MIDI_Eb7  :
      incr = MUSIC_Eb7;
      break;
    case MIDI_E7  :
      incr = MUSIC_E7;
      break;
    case MIDI_F7  :
      incr = MUSIC_F7;
      break;
    case MIDI_Gb7  :
      incr = MUSIC_Gb7;
      break;
    case MIDI_G7  :
      incr = MUSIC_G7;
      break;
    case MIDI_Ab7  :
      incr = MUSIC_Ab7;
      break;
    case MIDI_A7  :
      incr = MUSIC_A7;
      break;
    case MIDI_Bb7  :
      incr = MUSIC_Bb7;
      break;
    case MIDI_B7  :
      incr = MUSIC_B7;
      break;
    case MIDI_C8  :
      incr = MUSIC_C8;
      break;
    case MIDI_Db8  :
      incr = MUSIC_Db8;
      break;
    case MIDI_D8  :
      incr = MUSIC_D8;
      break;
    case MIDI_Eb8  :
      incr = MUSIC_Eb8;
      break;
    case MIDI_E8  :
      incr = MUSIC_E8;
      break;
    case MIDI_F8  :
      incr = MUSIC_F8;
      break;
    case MIDI_Gb8  :
      incr = MUSIC_Gb8;
      break;
    case MIDI_G8  :
      incr = MUSIC_G8;
      break;
    case MIDI_Ab8  :
      incr = MUSIC_Ab8;
      break;
    case MIDI_A8  :
      incr = MUSIC_A8;
      break;
    case MIDI_Bb8  :
      incr = MUSIC_Bb8;
      break;
    case MIDI_B8  :
      incr = MUSIC_B8;
      break;
    default :
      incr = MUSIC_C0;
  }

  for (int i = 0; i < INCREMENT_TABLE_SIZE; i++)
  {
    runningIncr += incr;  // keep track of the real floating point value of the incr
    osc.incrTable[i] = round (runningIncr) - round (runningIncr - incr);  // put the integer difference between the new rounded running total and the last 
  }

  osc.incrTableNeedsProc = 0;
}

ISR (TIMER1_COMPA_vect)
{
  SPIDAC_MasterTransmit ( currentWave[osc.tablePos] * osc.ampVal ); // output wave at current table position
  osc.tablePos = ( osc.tablePos + osc.incrTable[osc.incrTablePos] ) % INITIAL_WAVE_TABLE_SIZE; // update the table position using the incrTable and ensure it wraps around the wave table
  osc.incrTablePos = (osc.incrTablePos + 1) % INCREMENT_TABLE_SIZE; // update the increment table increment and make sure it wraps around the incr table
}
