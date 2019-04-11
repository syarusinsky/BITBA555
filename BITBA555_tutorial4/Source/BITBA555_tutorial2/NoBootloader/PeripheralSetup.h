#define SAMPLE_RATE 16000 // sampling at this frequency - 16 kHz
#define MIDI_BAUD_RATE 31250

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

  uint8_t dataPacket1 = 0b00110000 | (data >> 8); // write configuration bits for MCP4921 and OR with 4 MSB of 12 bit data
  
  SPDR = dataPacket1;  // put dataPacket1 in SPI data register
  while ( !(SPSR & (1 << SPIF)) );  // wait for transfer to complete

  uint8_t dataPacket2 = (data & 0b0000000011111111);  // put 8 LSB into dataPacket2

  SPDR = dataPacket2; // put dataPacket2 in SPI data register
  while ( !(SPSR & (1 << SPIF)) );  // wait for transfer to complete

  PORTB |= (1 << PINB2);  // pull CS high to end data transfer
}
