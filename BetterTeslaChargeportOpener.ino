// BetterTeslaChargeportOpener 
// Copyright 09/2020, Stefan Thesen
// License BSD-3
//
// Opens the charge port door of the Model3 (maybe other models too) with a 433MHz transmission.
// Existing codes on the net did not work, so I developed this one. The timing has been sniffed 
// from a original Tesla Gen2 Universal Mobile Connector using a 434MHz receiver and a logic analyzer.
//
// Runs on an Arduino Nano or similar
// What does it do: Signal is sent out on start of the Arduino once; the Arduino blinks the builtin LED upon completion
// How to connect: Connect a good 433 MHz ASK Transmitter: D10: 5V (switched), D11 Data and do not forget to connect GND
//
// Notes:
// Pulses from the original Tesla Connector are not really exact. There is a random jitter of around 10ms. It though seems that 
// the single pulses shall be 400us long. In the preamble though there is a kind of "chirp". The original connector varies the 
// length of the high states roughly from 530us to 400us and the low states from 270us to 400us over 13 repetitions. I do not
// know if this "chirp" in the preamble is really relevant or rather a Tesla bug. I built it in for compatibility.

// Pins
int iPowerPin = 10;           // where we supply power for the transmitter
int iSigPin = 11;             // pin where the signal comes out

// Signal definitions
int iTransmits=10;            // how many times do we send the whole code
int iTransmitDelay_ms=25;     // delay between transmits
int iManClock_us=400;         // Manchester Clock and length base of all pulses
int iChirp_us=10;             // change of length between pulses on preamble "chirp"

// Manchester Code
#define iManLength 6
uint8_t bManCode[iManLength] = {0x48,0xa9,0xca,0xe6,0xd8,0xc2};

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  pinMode(iSigPin, OUTPUT);
  digitalWrite(iSigPin, 0);
  pinMode(iPowerPin, OUTPUT);
  digitalWrite(iPowerPin, 0);
}

//creates the preamble before the 3 repetitions of the open command
void SendPreamble()
{
  // transmit preamble: 13 times high to low + add the "chirp" on top
  for (int ii=0;ii<13;ii++)
  {
    digitalWrite(iSigPin, 1);
    delayMicroseconds(iManClock_us+(12-ii)*iChirp_us);
    digitalWrite(iSigPin, 0);
    delayMicroseconds(iManClock_us-(12-ii)*iChirp_us);
  }

  // space between preamble and Manchester Code
  delayMicroseconds(iManClock_us);
}

// just a simple manchester encoder to avoid addtl libs
void SendManchester()
{
  int iByte,iBit;

  for (iByte=0;iByte<iManLength;iByte++)
  {
    int cValue=bManCode[iByte];
    for(iBit=0;iBit<8;iBit++)
    {
      int bBitVal=0;
      if ( (cValue & (1<<iBit)) != 0) 
      {
        // transmit one; i.e. we need a negative edge
        digitalWrite(iSigPin, 1);
        delayMicroseconds(iManClock_us);
        digitalWrite(iSigPin, 0);
        delayMicroseconds(iManClock_us);
      }
      else
      {
        // transmit zero; i.e. we need a positive edge
        digitalWrite(iSigPin, 0);
        delayMicroseconds(iManClock_us);
        digitalWrite(iSigPin, 1);
        delayMicroseconds(iManClock_us);
      }    
    }
  }
}

// creates the postamble after each of the three repetions of the signal
// note that the lasts repetition is a bit different 
void SendPostamble(bool bLast)
{
   // send Postamble
    digitalWrite(iSigPin, 0);         // 400us low
    delayMicroseconds(iManClock_us);
    digitalWrite(iSigPin, 1);         // 400us high
    delayMicroseconds(iManClock_us);
    digitalWrite(iSigPin, 0);         // 400us low
    delayMicroseconds(iManClock_us);
    
    int iMulti=2;
    if(bLast) {iMulti=1;} 
    digitalWrite(iSigPin, 1);         // 800us high, 400us on last repetition
    delayMicroseconds(iMulti*iManClock_us);
    digitalWrite(iSigPin, 0);         // 800us low
    delayMicroseconds(iManClock_us);
}

void loop() 
{
  digitalWrite(iPowerPin, 1); // tx power on

  // message repetition loop
  for (int iTransmit=0; iTransmit<iTransmits;iTransmit++)
  {  
    SendPreamble();
  
    // repeat Manchester & Postamble 3 times
    for (int iRepeat=0;iRepeat<3;iRepeat++)
    {
      // Manchester Code & Postamble
      SendManchester();
      SendPostamble(iRepeat==2);
    }
    
    delay(iTransmitDelay_ms); // 25ms delay between transmissions
  }
  digitalWrite(iPowerPin, 0); // tx power off

  //blink when done
  for (int i=0;i<10;i++)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }

  // endless loop
  while(1){};
}
