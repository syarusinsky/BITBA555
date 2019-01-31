/*

Compiling this C code will result in the .hex and .elf files included, and will allow you to program the ATmega328p without the Arduino bootloader.
The easiest way to compile this code would be to download the Atmel Studio IDE, create a new project with the ATmega328p as the target, and paste
this file into main.c. Alternatively, you could compile this code on the command line using the AVR 8-bit GNU toolchain. The toolchain can be found
here (https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers).

This is specifically for people who don't want to use the Arduino development board and IDE.

*/

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL	// clock rate of ATmega328p
#define SAMPLE_RATE 16000 // sampling at this frequency - 16 kHz

#define MIDI_BAUD_RATE 31250
#define MIDI_NOTE_OFF 0b1000
#define MIDI_NOTE_ON 0b1001
#define MIDI_EVENT_BUFFER_SIZE 10

volatile typedef struct midiEvent {  // Midi Event struct used for note on and note off messages
	uint8_t statusByte;
	uint8_t dataByte[2];
} MidiEvent;

MidiEvent midiEventBuffer[MIDI_EVENT_BUFFER_SIZE];  // a buffer to store MidiEvents for main loop to process
uint8_t midiReadIndex = 0;  // used by main loop to keep track of current read
volatile uint8_t midiWriteIndex = 0; // used by usart interrupt to keep track of current write, when midiReadIndex = midiWriteIndex then all data in buffer has been processed
volatile uint8_t midiWritingFlag = 255;  // if 255 then not currently writing data bytes, if 0 then writing dataByte 1, if 1 then writing dataByte 2

volatile uint16_t waveIncr = 0;

void Timer1Initialize();
void USART_Init();
void USART_Transmit(uint8_t data);
void SPIDAC_MasterInit();
void SPIDAC_MasterTransmit(uint16_t data);

int main(void)
{
	cli();  // disable interrupts
	Timer1Initialize();
	USART_Init();
	SPIDAC_MasterInit();
	sei();  // enable interrupts
	
    while (1) 
    {
		// use this for tutorial
		while ( midiReadIndex != midiWriteIndex )  // if there are unprocessed midi events in midi event buffer
		{
			USART_Transmit ( midiEventBuffer[midiReadIndex].statusByte );
			USART_Transmit ( midiEventBuffer[midiReadIndex].dataByte[0] );
			USART_Transmit ( midiEventBuffer[midiReadIndex].dataByte[1] );
			midiReadIndex = (midiReadIndex + 1) % MIDI_EVENT_BUFFER_SIZE;
		}
    }
}

ISR (TIMER1_COMPA_vect)
{
	// use this for tutorial (will create a 320Hz square wave)
	if (waveIncr < 25)
	{
		SPIDAC_MasterTransmit (4095);
		waveIncr++;
	} else if (waveIncr < 49)
	{
		SPIDAC_MasterTransmit (0);
		waveIncr++;
	} else
	{
		SPIDAC_MasterTransmit (0);
		waveIncr = 0;
	}
}

ISR (USART_RX_vect) // interrupt service routine called each time USART data is recieved
{
	uint8_t data = UDR0;  // receives data from USART data register
	uint8_t midiMessageType = (data >> 4);

	if ( midiMessageType == MIDI_NOTE_ON || midiMessageType == MIDI_NOTE_OFF )  // if we recieve a midi note on message or a midi note off message, begin writing to the midi buffer
	{
		midiEventBuffer[midiWriteIndex].statusByte = data;  // write the status byte to the current midi write index
		midiWritingFlag = 0;  // set the writing flag to begin writing data bytes
	} else if ( midiWritingFlag < 1 ) // we're only concerned with 2 data bytes, but if we were accepting messages of different lengths this value would have to be determined dynamically in the previous block
	{
		midiEventBuffer[midiWriteIndex].dataByte[midiWritingFlag] = data; // write data bytes to current midi write index
		midiWritingFlag++;  // increment the midi writing flag in order to write the next data byte
	} else if ( midiWritingFlag < 255 )
	{
		midiEventBuffer[midiWriteIndex].dataByte[midiWritingFlag] = data; // write data bytes to current midi write index

		uint8_t currentStatusByte = midiEventBuffer[midiWriteIndex].statusByte;
		midiWriteIndex = (midiWriteIndex + 1) % MIDI_EVENT_BUFFER_SIZE; // increment the midi write index since we've completed a midi event, use modulo operator for circular buffer
		
		midiEventBuffer[midiWriteIndex].statusByte = currentStatusByte;  // write the next midi event's status byte in case of running status
		midiWritingFlag = 0;  // set the writing flag to begin writing data bytes in case of running status
	}
}

void Timer1Initialize ()
{
	TCCR1A = 0; // set normal port operation
	TCCR1B = 0; // set to normal counter operation

	TCCR1B |= (1 << CS10) | (1 << WGM12); // set to CTC mode with WGM12, and set no prescaler with CS10

	TCNT1 = 0;  // set TCNT1 to 0, since it will be compared with output compare registers and be used to generate an output compare interrupt
	TIMSK1 |= (1 << OCIE1A);  // enable compare match interrupt
	
	OCR1A = ( F_CPU / SAMPLE_RATE ) - 1;  // setting the output compare register to count to value specified by cpu frequency and sample rate in order to interrupt at correct sample rate
}

void USART_Init ()
{
	uint16_t UBRR = ( F_CPU / 16 / MIDI_BAUD_RATE ) - 1; // refer to ATmega328p datasheet page 182 for derivation

	UBRR0H = (uint8_t) (UBRR >> 8);
	UBRR0L = (uint8_t) (UBRR);
	
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // configuring USART (RXEN = reciever enable, TXEN = transmitter enable, RXCIE = reciever interrupt enable
	
	UCSR0C |=  (1 << UCSZ01) | (1 << UCSZ00) ;  // setting the size of data in bits (UCSZ00 and UCSZ01 = 8 bits)
}

void USART_Transmit (uint8_t data)
{
	while ( !( UCSR0A & (1<<UDRE0)) );  // wait until tranfer ready

	UDR0 = data;  // put data in USART data register
}

void SPIDAC_MasterInit ()
{
	DDRB |= (1 << PINB5) | (1 << PINB3) | (1 << PINB2); // configuring serial pins as outputs (PINB5 = SCK, PINB3 = MOSI, PINB2 = CS)

	PORTB |= (1 << PINB2);  // initialize CS as high, since we won't be immediately writing to the DAC

	SPCR = (1 << SPE) | (1 << MSTR);  // initializing SPCR (SPE = SPI enable, MSTR = SPI master, SPI2X and SPR1 and SPR0 = set SPI clock to F_CPU/4)
}

void SPIDAC_MasterTransmit (uint16_t data)
{
	PORTB &= ~(1 << PINB2); // pull CS low to start data transfer

	uint8_t dataPacket1 = 0b00110000 | (data >> 8); // write configuration bits for MCP4921 and OR with 4 MSB of 12 bit data (extremely important that input data isn't over 4095, or config bits will be messed up)
	
	SPDR = dataPacket1;  // put dataPacket1 in SPI data register
	while ( !(SPSR & (1 << SPIF)) );  // wait for transfer to complete

	uint8_t dataPacket2 = (data & 0b0000000011111111);  // put 8 LSB into dataPacket2

	SPDR = dataPacket2; // put dataPacket2 in SPI data register
	while ( !(SPSR & (1 << SPIF)) );  // wait for transfer to complete

	PORTB |= (1 << PINB2);  // pull CS high to end data transfer
}