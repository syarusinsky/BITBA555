#include <math.h>

#define INITIAL_WAVE_FREQUENCY 100 // one cycle of each waveform will be stored in the buffers will at this frequency in Hz
#define INITIAL_WAVE_TABLE_SIZE (SAMPLE_RATE / INITIAL_WAVE_FREQUENCY) // the waveforms stored in the buffers will need arrays of this size
#define M_PI 3.14159265359  // pi, though some of the end may be truncated

// correct floating point increment for each musical frequency (freq / sample rate gives us cycles per sample
// but then we have to multiply by the initial wave table size since 1 cycle is now (sample rate / initial wave frequency) samples
#define MUSIC_C0 INITIAL_WAVE_TABLE_SIZE * (16.35 / SAMPLE_RATE)
#define MUSIC_Db0 INITIAL_WAVE_TABLE_SIZE * (17.32 / SAMPLE_RATE)
#define MUSIC_D0 INITIAL_WAVE_TABLE_SIZE * (18.35 / SAMPLE_RATE)
#define MUSIC_Eb0 INITIAL_WAVE_TABLE_SIZE * (19.45 / SAMPLE_RATE)
#define MUSIC_E0 INITIAL_WAVE_TABLE_SIZE * (20.60 / SAMPLE_RATE)
#define MUSIC_F0 INITIAL_WAVE_TABLE_SIZE * (21.83 / SAMPLE_RATE)
#define MUSIC_Gb0 INITIAL_WAVE_TABLE_SIZE * (23.12 / SAMPLE_RATE)
#define MUSIC_G0 INITIAL_WAVE_TABLE_SIZE * (24.50 / SAMPLE_RATE)
#define MUSIC_Ab0 INITIAL_WAVE_TABLE_SIZE * (25.96 / SAMPLE_RATE)
#define MUSIC_A0 INITIAL_WAVE_TABLE_SIZE * (27.50 / SAMPLE_RATE)
#define MUSIC_Bb0 INITIAL_WAVE_TABLE_SIZE * (29.14 / SAMPLE_RATE)
#define MUSIC_B0 INITIAL_WAVE_TABLE_SIZE * (30.87 / SAMPLE_RATE)
#define MUSIC_C1 INITIAL_WAVE_TABLE_SIZE * (32.70 / SAMPLE_RATE)
#define MUSIC_Db1 INITIAL_WAVE_TABLE_SIZE * (34.65 / SAMPLE_RATE)
#define MUSIC_D1 INITIAL_WAVE_TABLE_SIZE * (36.71 / SAMPLE_RATE)
#define MUSIC_Eb1 INITIAL_WAVE_TABLE_SIZE * (38.89 / SAMPLE_RATE)
#define MUSIC_E1 INITIAL_WAVE_TABLE_SIZE * (41.20 / SAMPLE_RATE)
#define MUSIC_F1 INITIAL_WAVE_TABLE_SIZE * (43.65 / SAMPLE_RATE)
#define MUSIC_Gb1 INITIAL_WAVE_TABLE_SIZE * (46.25 / SAMPLE_RATE)
#define MUSIC_G1 INITIAL_WAVE_TABLE_SIZE * (49.00 / SAMPLE_RATE)
#define MUSIC_Ab1 INITIAL_WAVE_TABLE_SIZE * (51.91 / SAMPLE_RATE)
#define MUSIC_A1 INITIAL_WAVE_TABLE_SIZE * (55.00 / SAMPLE_RATE)
#define MUSIC_Bb1 INITIAL_WAVE_TABLE_SIZE * (58.27 / SAMPLE_RATE)
#define MUSIC_B1 INITIAL_WAVE_TABLE_SIZE * (61.74 / SAMPLE_RATE)
#define MUSIC_C2 INITIAL_WAVE_TABLE_SIZE * (65.41 / SAMPLE_RATE)
#define MUSIC_Db2 INITIAL_WAVE_TABLE_SIZE * (69.30 / SAMPLE_RATE)
#define MUSIC_D2 INITIAL_WAVE_TABLE_SIZE * (73.42 / SAMPLE_RATE)
#define MUSIC_Eb2 INITIAL_WAVE_TABLE_SIZE * (77.78 / SAMPLE_RATE)
#define MUSIC_E2 INITIAL_WAVE_TABLE_SIZE * (82.41 / SAMPLE_RATE)
#define MUSIC_F2 INITIAL_WAVE_TABLE_SIZE * (87.31 / SAMPLE_RATE)
#define MUSIC_Gb2 INITIAL_WAVE_TABLE_SIZE * (92.50 / SAMPLE_RATE)
#define MUSIC_G2 INITIAL_WAVE_TABLE_SIZE * (98.00 / SAMPLE_RATE)
#define MUSIC_Ab2 INITIAL_WAVE_TABLE_SIZE * (103.83 / SAMPLE_RATE)
#define MUSIC_A2 INITIAL_WAVE_TABLE_SIZE * (110.00 / SAMPLE_RATE)
#define MUSIC_Bb2 INITIAL_WAVE_TABLE_SIZE * (116.54 / SAMPLE_RATE)
#define MUSIC_B2 INITIAL_WAVE_TABLE_SIZE * (123.47 / SAMPLE_RATE)
#define MUSIC_C3 INITIAL_WAVE_TABLE_SIZE * (130.81 / SAMPLE_RATE)
#define MUSIC_Db3 INITIAL_WAVE_TABLE_SIZE * (138.59 / SAMPLE_RATE)
#define MUSIC_D3 INITIAL_WAVE_TABLE_SIZE * (146.83 / SAMPLE_RATE)
#define MUSIC_Eb3 INITIAL_WAVE_TABLE_SIZE * (155.56 / SAMPLE_RATE)
#define MUSIC_E3 INITIAL_WAVE_TABLE_SIZE * (164.81 / SAMPLE_RATE)
#define MUSIC_F3 INITIAL_WAVE_TABLE_SIZE * (174.61 / SAMPLE_RATE)
#define MUSIC_Gb3 INITIAL_WAVE_TABLE_SIZE * (185.00 / SAMPLE_RATE)
#define MUSIC_G3 INITIAL_WAVE_TABLE_SIZE * (196.00 / SAMPLE_RATE)
#define MUSIC_Ab3 INITIAL_WAVE_TABLE_SIZE * (207.65 / SAMPLE_RATE)
#define MUSIC_A3 INITIAL_WAVE_TABLE_SIZE * (220.00 / SAMPLE_RATE)
#define MUSIC_Bb3 INITIAL_WAVE_TABLE_SIZE * (233.08 / SAMPLE_RATE)
#define MUSIC_B3 INITIAL_WAVE_TABLE_SIZE * (246.94 / SAMPLE_RATE)
#define MUSIC_C4 INITIAL_WAVE_TABLE_SIZE * (261.63 / SAMPLE_RATE)
#define MUSIC_Db4 INITIAL_WAVE_TABLE_SIZE * (277.18 / SAMPLE_RATE)
#define MUSIC_D4 INITIAL_WAVE_TABLE_SIZE * (293.66 / SAMPLE_RATE)
#define MUSIC_Eb4 INITIAL_WAVE_TABLE_SIZE * (311.13 / SAMPLE_RATE)
#define MUSIC_E4 INITIAL_WAVE_TABLE_SIZE * (329.63 / SAMPLE_RATE)
#define MUSIC_F4 INITIAL_WAVE_TABLE_SIZE * (349.23 / SAMPLE_RATE)
#define MUSIC_Gb4 INITIAL_WAVE_TABLE_SIZE * (369.99 / SAMPLE_RATE)
#define MUSIC_G4 INITIAL_WAVE_TABLE_SIZE * (392.00 / SAMPLE_RATE)
#define MUSIC_Ab4 INITIAL_WAVE_TABLE_SIZE * (415.30 / SAMPLE_RATE)
#define MUSIC_A4 INITIAL_WAVE_TABLE_SIZE * (440.00 / SAMPLE_RATE)
#define MUSIC_Bb4 INITIAL_WAVE_TABLE_SIZE * (466.16 / SAMPLE_RATE)
#define MUSIC_B4 INITIAL_WAVE_TABLE_SIZE * (493.88 / SAMPLE_RATE)
#define MUSIC_C5 INITIAL_WAVE_TABLE_SIZE * (523.25 / SAMPLE_RATE)
#define MUSIC_Db5 INITIAL_WAVE_TABLE_SIZE * (554.37 / SAMPLE_RATE)
#define MUSIC_D5 INITIAL_WAVE_TABLE_SIZE * (587.33 / SAMPLE_RATE)
#define MUSIC_Eb5 INITIAL_WAVE_TABLE_SIZE * (622.25 / SAMPLE_RATE)
#define MUSIC_E5 INITIAL_WAVE_TABLE_SIZE * (659.25 / SAMPLE_RATE)
#define MUSIC_F5 INITIAL_WAVE_TABLE_SIZE * (698.46 / SAMPLE_RATE)
#define MUSIC_Gb5 INITIAL_WAVE_TABLE_SIZE * (739.99 / SAMPLE_RATE)
#define MUSIC_G5 INITIAL_WAVE_TABLE_SIZE * (783.99 / SAMPLE_RATE)
#define MUSIC_Ab5 INITIAL_WAVE_TABLE_SIZE * (830.61 / SAMPLE_RATE)
#define MUSIC_A5 INITIAL_WAVE_TABLE_SIZE * (880.00 / SAMPLE_RATE)
#define MUSIC_Bb5 INITIAL_WAVE_TABLE_SIZE * (932.33 / SAMPLE_RATE)
#define MUSIC_B5 INITIAL_WAVE_TABLE_SIZE * (987.77 / SAMPLE_RATE)
#define MUSIC_C6 INITIAL_WAVE_TABLE_SIZE * (1046.50 / SAMPLE_RATE)
#define MUSIC_Db6 INITIAL_WAVE_TABLE_SIZE * (1108.73 / SAMPLE_RATE)
#define MUSIC_D6 INITIAL_WAVE_TABLE_SIZE * (1174.66 / SAMPLE_RATE)
#define MUSIC_Eb6 INITIAL_WAVE_TABLE_SIZE * (1244.51 / SAMPLE_RATE)
#define MUSIC_E6 INITIAL_WAVE_TABLE_SIZE * (1318.51 / SAMPLE_RATE)
#define MUSIC_F6 INITIAL_WAVE_TABLE_SIZE * (1396.91 / SAMPLE_RATE)
#define MUSIC_Gb6 INITIAL_WAVE_TABLE_SIZE * (1479.98 / SAMPLE_RATE)
#define MUSIC_G6 INITIAL_WAVE_TABLE_SIZE * (1567.98 / SAMPLE_RATE)
#define MUSIC_Ab6 INITIAL_WAVE_TABLE_SIZE * (1661.22 / SAMPLE_RATE)
#define MUSIC_A6 INITIAL_WAVE_TABLE_SIZE * (1760.00 / SAMPLE_RATE)
#define MUSIC_Bb6 INITIAL_WAVE_TABLE_SIZE * (1864.66 / SAMPLE_RATE)
#define MUSIC_B6 INITIAL_WAVE_TABLE_SIZE * (1975.53 / SAMPLE_RATE)
#define MUSIC_C7 INITIAL_WAVE_TABLE_SIZE * (2093.00 / SAMPLE_RATE)
#define MUSIC_Db7 INITIAL_WAVE_TABLE_SIZE * (2217.46 / SAMPLE_RATE)
#define MUSIC_D7 INITIAL_WAVE_TABLE_SIZE * (2349.32 / SAMPLE_RATE)
#define MUSIC_Eb7 INITIAL_WAVE_TABLE_SIZE * (2489.02 / SAMPLE_RATE)
#define MUSIC_E7 INITIAL_WAVE_TABLE_SIZE * (2637.02 / SAMPLE_RATE)
#define MUSIC_F7 INITIAL_WAVE_TABLE_SIZE * (2793.83 / SAMPLE_RATE)
#define MUSIC_Gb7 INITIAL_WAVE_TABLE_SIZE * (2959.96 / SAMPLE_RATE)
#define MUSIC_G7 INITIAL_WAVE_TABLE_SIZE * (3135.96 / SAMPLE_RATE)
#define MUSIC_Ab7 INITIAL_WAVE_TABLE_SIZE * (3322.44 / SAMPLE_RATE)
#define MUSIC_A7 INITIAL_WAVE_TABLE_SIZE * (3520.00 / SAMPLE_RATE)
#define MUSIC_Bb7 INITIAL_WAVE_TABLE_SIZE * (3729.31 / SAMPLE_RATE)
#define MUSIC_B7 INITIAL_WAVE_TABLE_SIZE * (3951.07 / SAMPLE_RATE)
#define MUSIC_C8 INITIAL_WAVE_TABLE_SIZE * (4186.01 / SAMPLE_RATE)
#define MUSIC_Db8 INITIAL_WAVE_TABLE_SIZE * (4434.92 / SAMPLE_RATE)
#define MUSIC_D8 INITIAL_WAVE_TABLE_SIZE * (4698.63 / SAMPLE_RATE)
#define MUSIC_Eb8 INITIAL_WAVE_TABLE_SIZE * (4978.03 / SAMPLE_RATE)
#define MUSIC_E8 INITIAL_WAVE_TABLE_SIZE * (5274.04 / SAMPLE_RATE)
#define MUSIC_F8 INITIAL_WAVE_TABLE_SIZE * (5587.65 / SAMPLE_RATE)
#define MUSIC_Gb8 INITIAL_WAVE_TABLE_SIZE * (5919.91 / SAMPLE_RATE)
#define MUSIC_G8 INITIAL_WAVE_TABLE_SIZE * (6271.93 / SAMPLE_RATE)
#define MUSIC_Ab8 INITIAL_WAVE_TABLE_SIZE * (6644.88 / SAMPLE_RATE)
#define MUSIC_A8 INITIAL_WAVE_TABLE_SIZE * (7040.00 / SAMPLE_RATE)
#define MUSIC_Bb8 INITIAL_WAVE_TABLE_SIZE * (7458.62 / SAMPLE_RATE)
#define MUSIC_B8 INITIAL_WAVE_TABLE_SIZE * (7902.13 / SAMPLE_RATE)

uint8_t waveBuffer[INITIAL_WAVE_TABLE_SIZE * 4];  // the waveBuffer holds all waveforms of INITIAL_WAVE_FREQUENCY
uint8_t* sineWave = &waveBuffer[0]; // a pointer to the beginning of the sine waveform
uint8_t* triangleWave = &waveBuffer[INITIAL_WAVE_TABLE_SIZE]; // a pointer to the beginning of the triangle waveform
uint8_t* squareWave = &waveBuffer[INITIAL_WAVE_TABLE_SIZE * 2]; // a pointer to the beginning of the square waveform
uint8_t* sawtoothWave = &waveBuffer[INITIAL_WAVE_TABLE_SIZE * 3]; // a pointer to the beginning of the sawtooth waveform
uint8_t* currentWave = sineWave;

// sine wave y(t) = Asin(2piFt + p) : A = amplitude, F = frequency, t = time, p = phase
void generateSine ()
{
  const float cyclesPerSample = ((float)INITIAL_WAVE_FREQUENCY / (float)SAMPLE_RATE); // this is the frequency we're concerned with, instead of cycles per second (Hz)

  for (int i = 0; i < INITIAL_WAVE_TABLE_SIZE; i++)
  {
    sineWave[i] = round ( (255.0/2) * (1.0 + sin (2.0 * M_PI * cyclesPerSample * i + (3* M_PI) / 2)) ); // sinewave = Asin(2 * pi * freq * time + phase), with freq in cyclesPerSample
    // a phase of (3pi/2) is added so that the signal starts at 0 instead of 128, which would make a popping sound when the key was first pressed
  }
}

void generateTriangle ()
{
  const float incr = 255.0 / (INITIAL_WAVE_TABLE_SIZE / 2.0); // the amount to increment or decrement for each sample
  const int decrPoint = ceil (INITIAL_WAVE_TABLE_SIZE / 2.0);  // at this point in the samples, we begin to decrement instead of increment to achieve triangle waveform

  triangleWave[0] = 0;
  
  float tempIncr = 0; // a temporary variable we store to keep track of the correct floating point value of triangleWave[i]
  for (int i = 1; i < INITIAL_WAVE_TABLE_SIZE; i++)
  {
    if (i <= decrPoint)
    {
      tempIncr += incr; 
      triangleWave[i] = floor (tempIncr);
    }
    else
    {
      tempIncr -= incr; 
      triangleWave[i] = floor (tempIncr);
    }
  }
}

void generateSquare ()
{
  const int flipPoint = ceil(INITIAL_WAVE_TABLE_SIZE / 2.0);  // at this point in the samples, output low instead of high to achieve square waveform
  
  for (int i = 0; i < INITIAL_WAVE_TABLE_SIZE; i++)
  {
    if (i < flipPoint)
    {
      squareWave[i] = 255;
    }
    else
    {
      squareWave[i] = 0;
    }
  }
}

void generateSawtooth ()
{
  const float incr = (255.0 / INITIAL_WAVE_TABLE_SIZE);
  
  for (int i = 0; i < INITIAL_WAVE_TABLE_SIZE; i++)
  {
    sawtoothWave[i] = incr * i;
  }
}
