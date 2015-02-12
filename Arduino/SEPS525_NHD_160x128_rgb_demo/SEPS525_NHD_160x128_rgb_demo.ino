//--------------------------------------------------------------------------
/*
Annotated and updated by Martin Falatic

Portions (c)2014 Mike LaVine - Newhaven Display International, LLC. 
http://www.newhavendisplay.com/NHD_forum/index.php/topic,64.0.html

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
//--------------------------------------------------------------------------
//##########################################################################
//--------------------------------------------------------------------------

#include <SPI.h>

#define MODE_3WIRE    1   // BS1=0, BS0 = 1
#define MODE_4WIRE    2   // BS1=0, BS0 = 0, needs the RS (D/C) signal
#define MODE_SPI4W    3   // BS1=0, BS0 = 0, needs the RS (D/C) signal

#define SEND_CMD      1   // 3- and 4-wire - Display instruction (command)
#define SEND_DAT      2   // 3- and 4-wire - Display instruction (data)

#define MAXROWS      64   // Still figuring these out...
#define MAXCOLS     240   // Still figuring these out...

// Pin mappings for Mega2560
#define PIN_SCLK  30  // SCLK signal (SPI uses SCK  on pin 52)
#define PIN_SDIN  31  // SDIN signal (SPI uses MOSI on pin 51)
#define PIN_RS    32  // RS (D/C) signal (can be tied low for 3-wire SPI)
#define PIN_CS    33  // /CS signal (certain SPI can use pin 53)
                      // (can be tied low with a single display)
#define PIN_RES   34  // /RES signal

int SIG_MODE = MODE_4WIRE;
//int SIG_MODE = MODE_3WIRE;
//int SIG_MODE = MODE_SPI4W;

//--------------------------------------------------------------------------
//##########################################################################
//--------------------------------------------------------------------------

// This is slow. Really slow. (At 16 MHz anyway)
//#define digitalPinSetVal(IOPTR, VAL) ( digitalWrite((IOPTR)->pin, VAL) )

// Much more efficient!
#define digitalPinSetVal(IOPTR, VAL) ( (VAL == LOW) ? \
                                        (*(IOPTR)->reg &= ~(IOPTR)->mask) : \
                                        (*(IOPTR)->reg |=  (IOPTR)->mask) )

struct IOMAP_Struct
{
  uint8_t pin;
  volatile uint8_t * reg;
  uint8_t mask;
} IOMAP_SCLK, IOMAP_SDIN, IOMAP_RS, IOMAP_RES, IOMAP_CS;

void InitPin(struct IOMAP_Struct * IOMAP_temp, uint8_t pin)
{
  IOMAP_temp->pin  = pin;
  uint8_t port = digitalPinToPort(pin);
  if (port != NOT_A_PIN)
  {
    IOMAP_temp->reg  = portOutputRegister(port);
    IOMAP_temp->mask = digitalPinToBitMask(pin);
    // The following is equivalent to "pinMode(pin, OUTPUT)"
    volatile uint8_t * mode = portModeRegister(port);
    *mode |= IOMAP_temp->mask; // Output
  }
}

void InitStructsAndPins()
{
  InitPin(&IOMAP_SCLK, PIN_SCLK);
  InitPin(&IOMAP_SDIN, PIN_SDIN);
  InitPin(&IOMAP_RS,   PIN_RS);
  InitPin(&IOMAP_RES,  PIN_RES);
  InitPin(&IOMAP_CS,   PIN_CS);
}

//--------------------------------------------------------------------------
//##########################################################################
//--------------------------------------------------------------------------
void displaySend(uint8_t sendType, unsigned char v)
{
  unsigned char i;

  digitalPinSetVal(&IOMAP_CS, LOW);

  if (sendType == SEND_CMD)
  { // Send a command value
    if (SIG_MODE == MODE_4WIRE || SIG_MODE == MODE_SPI4W)
    {
      digitalPinSetVal(&IOMAP_RS, LOW);
    }
    else if (SIG_MODE == MODE_3WIRE)
    {
      digitalPinSetVal(&IOMAP_SDIN, LOW);
      digitalPinSetVal(&IOMAP_SCLK, LOW);
      digitalPinSetVal(&IOMAP_SCLK, HIGH);
    }
  }
  else if (sendType == SEND_DAT)
  { // Send a data value
    if (SIG_MODE == MODE_4WIRE || SIG_MODE == MODE_SPI4W)
    {
      digitalPinSetVal(&IOMAP_RS, HIGH);
    }
    else if (SIG_MODE == MODE_3WIRE)
    {
      digitalPinSetVal(&IOMAP_SDIN, HIGH);
      digitalPinSetVal(&IOMAP_SCLK, LOW);
      digitalPinSetVal(&IOMAP_SCLK, HIGH);
    }
  }

  if (SIG_MODE == MODE_3WIRE || SIG_MODE == MODE_4WIRE)
  {
    for(i=8;i>0;i--)
    { // Decrementing is faster
      digitalPinSetVal(&IOMAP_SCLK, LOW);
      if((v&0x80)>>7==1)
      {
        digitalPinSetVal(&IOMAP_SDIN, HIGH);
      }
      else
      {
        digitalPinSetVal(&IOMAP_SDIN, LOW);
      }
      v=v<<1;
      digitalPinSetVal(&IOMAP_SCLK, HIGH);
    }
  }
  else if (SIG_MODE == MODE_SPI4W) {
    SPI.transfer(v);
  }

  digitalPinSetVal(&IOMAP_CS, HIGH);
}

//--------------------------------------------------------------------------
void Set_Column_Address(unsigned char a, unsigned char b)
{
  displaySend(SEND_CMD, 0x15); // Set Column Address
  displaySend(SEND_DAT, a);    //   Default => 0x00
  displaySend(SEND_DAT, b);    //   Default => 0x77
}

//--------------------------------------------------------------------------
void Set_Row_Address(unsigned char a, unsigned char b)
{
  displaySend(SEND_CMD, 0x75); // Set Row Address
  displaySend(SEND_DAT, a);    //   Default => 0x00
  displaySend(SEND_DAT, b);    //   Default => 0x7F
}

//--------------------------------------------------------------------------
void Set_Write_RAM()
{
  displaySend(SEND_CMD, 0x5C); // Enable MCU to Write into RAM
}

//--------------------------------------------------------------------------
void Reset_Device()
{
  // Directly from the specs
  displaySend(SEND_CMD, 0x04); // 
  displaySend(SEND_DAT, 0x03); // = 
  delay(2);

  displaySend(SEND_CMD, 0x04); // 
  displaySend(SEND_DAT, 0x00); // = 
  delay(2);

  displaySend(SEND_CMD, 0x3B); // 
  displaySend(SEND_DAT, 0x00); // = 

  displaySend(SEND_CMD, 0x02); // 
  displaySend(SEND_DAT, 0x01); // = 

  displaySend(SEND_CMD, 0x03); // 
  displaySend(SEND_DAT, 0x90); // = 

  displaySend(SEND_CMD, 0x80); // 
  displaySend(SEND_DAT, 0x01); // = 

  displaySend(SEND_CMD, 0x08); // 
  displaySend(SEND_DAT, 0x04); // = 
  displaySend(SEND_CMD, 0x09); // 
  displaySend(SEND_DAT, 0x05); // = 
  displaySend(SEND_CMD, 0x0A); // 
  displaySend(SEND_DAT, 0x05); // = 

  displaySend(SEND_CMD, 0x0B); // 
  displaySend(SEND_DAT, 0x9D); // = 
  displaySend(SEND_CMD, 0x0C); // 
  displaySend(SEND_DAT, 0x8C); // = 
  displaySend(SEND_CMD, 0x0D); // 
  displaySend(SEND_DAT, 0x57); // = 

  displaySend(SEND_CMD, 0x10); // 
  displaySend(SEND_DAT, 0x56); // = 
  displaySend(SEND_CMD, 0x11); // 
  displaySend(SEND_DAT, 0x4D); // = 
  displaySend(SEND_CMD, 0x12); // 
  displaySend(SEND_DAT, 0x46); // = 

  displaySend(SEND_CMD, 0x13); // 
  displaySend(SEND_DAT, 0x0A); // = 

  displaySend(SEND_CMD, 0x14); // 
  displaySend(SEND_DAT, 0x01); // = 
  displaySend(SEND_CMD, 0x16); // 
  displaySend(SEND_DAT, 0x76); // = 

  displaySend(SEND_CMD, 0x20); // 
  displaySend(SEND_DAT, 0x00); // = 
  displaySend(SEND_CMD, 0x21); // 
  displaySend(SEND_DAT, 0x00); // = 

  displaySend(SEND_CMD, 0x28); // 
  displaySend(SEND_DAT, 0x7F); // = 

  displaySend(SEND_CMD, 0x29); // 
  displaySend(SEND_DAT, 0x00); // = 

  displaySend(SEND_CMD, 0x06); // 
  displaySend(SEND_DAT, 0x01); // = 

  displaySend(SEND_CMD, 0x05); // 
  displaySend(SEND_DAT, 0x00); // = 

  displaySend(SEND_CMD, 0x15); // 
  displaySend(SEND_DAT, 0x00); // = 

  delay(10);
}

//--------------------------------------------------------------------------
void ClearDisplay()
{
  unsigned int i, j;
  
  // Turn off display while clearing (also hides noise at powerup)
  displaySend(SEND_CMD, 0xA4); // Set Display Mode = OFF

  Set_Column_Address(0x00,0x77);
  Set_Row_Address(0x00,0x7F);
  Set_Write_RAM();

  for(i=0;i<MAXROWS;i++)
  {
    for(j=0;j<MAXCOLS/2;j++)
    {
      displaySend(SEND_DAT, 0x00);
      displaySend(SEND_DAT, 0x00);
    }
    for(j=0;j<MAXCOLS/2;j++)
    {
      displaySend(SEND_DAT, 0x00);
      displaySend(SEND_DAT, 0x00);
    }
  }

  displaySend(SEND_CMD, 0xA6); // Set Display Mode = Normal Display
}

//--------------------------------------------------------------------------
void FillDisplay()
{
  unsigned int i, j;
  
  Set_Column_Address(0x00,0x77);
  Set_Row_Address(0x00,0x7F);
  Set_Write_RAM();

  for(i=0;i<MAXROWS;i++)
  {
    for(j=0;j<MAXCOLS/2;j++)
    {
      displaySend(SEND_DAT, 0xFF);
      displaySend(SEND_DAT, 0xFF);
    }
    for(j=0;j<MAXCOLS/2;j++)
    {
      displaySend(SEND_DAT, 0xFF);
      displaySend(SEND_DAT, 0xFF);
    }
  }
}

//--------------------------------------------------------------------------
void CheckerboardOdd()
{
  unsigned int i, j;
  
  Set_Column_Address(0x00,0x77);
  Set_Row_Address(0x00,0x7F);
  Set_Write_RAM();

  for(i=0;i<MAXROWS;i++)
  {
    for(j=0;j<MAXCOLS/2;j++)
    {
      displaySend(SEND_DAT, 0x0F);
      displaySend(SEND_DAT, 0x0F);
    }
    for(j=0;j<MAXCOLS/2;j++)
    {
      displaySend(SEND_DAT, 0xF0);
      displaySend(SEND_DAT, 0xF0);
    }
  }
}

//--------------------------------------------------------------------------
void CheckerboardEven()
{
  unsigned int i, j;
  
  Set_Column_Address(0x00,0x77);
  Set_Row_Address(0x00,0x7F);
  Set_Write_RAM();

  for(i=0;i<MAXROWS;i++)
  {
    for(j=0;j<MAXCOLS/2;j++)
    {
      displaySend(SEND_DAT, 0xF0);
      displaySend(SEND_DAT, 0xF0);
    }
    for(j=0;j<MAXCOLS/2;j++)
    {
      displaySend(SEND_DAT, 0x0F);
      displaySend(SEND_DAT, 0x0F);
    }
  }
}

//--------------------------------------------------------------------------
//##########################################################################
//--------------------------------------------------------------------------
void setup()
{
  InitStructsAndPins();
  digitalPinSetVal(&IOMAP_RS,  LOW);
  digitalPinSetVal(&IOMAP_RES, HIGH);
  delay(1000);
  if (SIG_MODE == MODE_SPI4W) {
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    // DIV4 is almost as fast (for a 16 MHz device)
  }
  Reset_Device();
}

//--------------------------------------------------------------------------
void loop()
{ 
    //displaySend(SEND_CMD, 0xA4); // Entire Display OFF, all pixels turns OFF in GS level 0
    //displaySend(SEND_CMD, 0xA5); // Entire Display ON, all pixels turns ON in GS level 15
    //ClearDisplay();
    //CheckerboardOdd();
    //CheckerboardEven();
    //FillDisplay();
    delay(1000);
}

//--------------------------------------------------------------------------

