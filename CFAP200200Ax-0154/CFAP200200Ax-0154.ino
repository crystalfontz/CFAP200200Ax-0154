//=============================================================================
// "Arduino" example program for Crystalfontz ePaper. 
//
// This project is for the CFAP200200Ax-0154 :
//
//   https://www.crystalfontz.com/product/cfap200200A30154
//
// It was written against a Seeduino v4.2 @3.3v. An Arduino UNO modified to
// operate at 3.3v should also work.
//-----------------------------------------------------------------------------
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <http://unlicense.org/>
//=============================================================================
// Connecting the Arduino to the display
//
// ARDUINO |Adapter |Wire Color |Function
// --------+--------+-----------+--------------------
// D2      |19      |Yellow     |BS1 Not Used
// D3      |17      |Green      |Busy Line
// D4      |18      |Brown      |Reset Line
// D5      |15      |Purple     |Data/Command Line
// D10     |16      |Blue       |Chip Select Line
// D11     |14      |White      |MOSI
// D13     |13      |Orange     |Clock
// 3.3V    |5       |Red        |Power
// GND     |3       |Black      |Ground
//
// Short the following pins on the adapter board:
// GND  -> BS2
// RESE -> 3ohms
//=============================================================================
//Connecting the Arduino to the SD card
//A CFA10112 micro SD card adapter can be used: https://www.crystalfontz.com/product/cfa10112
//
// ARDUINO  |Wire Color |Function
// ---------+-----------+--------------------
// D2       |Blue       |CS
// D3       |Green      |MOSI
// D4       |Brown      |CLK
// D5       |Purple     |MISO
//
//
//=============================================================================
// Creating image data arrays
//
// Bmp_to_epaper is code that will aid in creating bitmaps necessary from .bmp files if desired.
// The code can be downloaded from the Crystalfontz website: 
//    https://www.crystalfontz.com/product/bmptoepaper-bitmap-to-epaper-software
// or it can be downloaded from github: 
//    https://github.com/crystalfontz/bmp_to_epaper
//=============================================================================

// The display is SPI, include the library header.
#include <SPI.h>
#include <SD.h>
#include <avr/io.h>

// Include LUTs
#include "LUTs_for_CFAP200200A30154.h"
//Include Images
#include "Images_for_CFAP200200A30154.h"


#define EPD_READY   3
#define EPD_RESET   4
#define EPD_DC      5
#define EPD_CS      10
#define SD_CS       8
#define EPD_MOSI    11
#define EPD_SCK     13

#define ePaper_RST_0  (digitalWrite(EPD_RESET, LOW))
#define ePaper_RST_1  (digitalWrite(EPD_RESET, HIGH))
#define ePaper_CS_0   (digitalWrite(EPD_CS, LOW))
#define ePaper_CS_1   (digitalWrite(EPD_CS, HIGH))
#define ePaper_DC_0   (digitalWrite(EPD_DC, LOW))
#define ePaper_DC_1   (digitalWrite(EPD_DC, HIGH))
#define ePaper_MOSI_0 (digitalWrite(EPD_MOSI, LOW))
#define ePaper_MOSI_1 (digitalWrite(EPD_MOSI, HIGH))
#define ePaper_SCK_0  (digitalWrite(EPD_SCK, LOW))
#define ePaper_SCK_1  (digitalWrite(EPD_SCK, HIGH))

#define HRES  200
#define VRES  200

#define WAIT_BUSY while (0 != digitalRead(EPD_READY))

//=============================================================================
//this function will take in a byte and send it to the display with the 
//command bit low for command transmission
void writeCMD(uint8_t command)
{

delay(1);
  ePaper_DC_0;
  ePaper_CS_0;
  SPI.transfer(command);
  ePaper_CS_1;
}

//this function will take in a byte and send it to the display with the 
//command bit high for data transmission
void writeData(uint8_t data)
{
  ePaper_DC_1;
  ePaper_CS_0;
  SPI.transfer(data);
  ePaper_CS_1;
}

void initEPD()
{
  //reset driver
  ePaper_RST_0;
  delay(1);
  ePaper_RST_1;
  delay(1);

  WAIT_BUSY
  writeCMD(0x12);  //SW Reset
  WAIT_BUSY;

  writeCMD(0x01); //Driver output control      
  writeData(0xC7);
  writeData(0x00);
  writeData(0x00);

  writeCMD(0x11); //data entry mode       
  writeData(0x01);

  writeCMD(0x44); //set Ram-X address start/end position   
  writeData(0x00);
  writeData(0x18);

  writeCMD(0x45); //set Ram-Y address start/end position          
  writeData(0xC7);
  writeData(0x00);
  writeData(0x00);
  writeData(0x00);

  writeCMD(0x3C); //BorderWaveform
  writeData(0x05);

  writeCMD(0x18);
  writeData(0x80);

  writeCMD(0x4E);
  writeData(0x00);
  writeCMD(0x4F);
  writeData(0xC7);
  writeData(0x00);
  WAIT_BUSY;
}

//================================================================================
void deepSleep()
{
  writeCMD(0x10);
  writeData(0x01);
  delay(200); //Put some delay in here so other commands aren't sent to it right away
}

//================================================================================
void loadFlashImage(uint16_t widthPixels,
  uint16_t heightPixels,
  const uint8_t* BW_image)
{

  //Make sure the display is not busy before starting a new command.
  WAIT_BUSY;

  //Write the command: Write Ram (Black White)
  writeCMD(0x24);

  for (int i = 0; i < widthPixels * heightPixels / 8; i++)
  {
    writeData(pgm_read_byte(&BW_image[i]));
  }

  //Aim back at the command register
  writeCMD(0x22);
  writeData(0xF7);
  writeCMD(0x20);
}

//================================================================================
void loadFlashImageBrush(uint16_t widthPixels,
  uint16_t heightPixels,
  const uint8_t* BW_image)
{

  //Make sure the display is not busy before starting a new command.
  WAIT_BUSY;

  //Write the command: Write Ram (Black White)
  writeCMD(0x24);

  for (int i = 0; i < widthPixels * heightPixels / 8; i++)
  {
    writeData(pgm_read_byte(&BW_image[i]));
  }

  //Write the command: Write Ram (Black White)
  writeCMD(0x26);

  for (int i = 0; i < widthPixels * heightPixels / 8; i++)
  {
    writeData(pgm_read_byte(&BW_image[i]));
  }

  //Aim back at the command register
  writeCMD(0x22);
  writeData(0xF7);
  writeCMD(0x20);
}

//================================================================================
//This function only works for displays <= 256 x 256 resolution
void loadPartialFlashImage(uint8_t widthPixels,
  uint8_t heightPixels,
  uint8_t x,
  uint8_t y,
  const uint8_t* BW_image)
{

  //convert from bits to bytes
  uint8_t xStart, width, xEnd, yStart, height, yEnd;
  xStart = x;
  yStart = y;
  width = widthPixels;
  height = heightPixels;

  xStart = xStart / 8;
  width = width / 8;

  yEnd = yStart + height;
  xEnd = xStart + width - 1;

  //Make sure the display is not busy before starting a new command.
  WAIT_BUSY;

    //reset driver
  //ePaper_RST_0;
  //delay(10);
  //ePaper_RST_1;
  //delay(10);

  //writeCMD(0x3c);
  //writeData(0x80);

  writeCMD(0x44);
  writeData(xStart);
  writeData(xEnd);
  writeCMD(0x45);
  writeData(yStart);
  writeData(0x00);
  writeData(yEnd);
  writeData(0x00);


  writeCMD(0x4E);   // set RAM x address count to 0;
  writeData(xStart);
  writeCMD(0x4F);   // set RAM y address count to 0X127;    
  writeData(yStart);
  writeData(0x00);

  //Write the command: Write Ram (Black White)
  writeCMD(0x24);


  for (int i = 0; i < widthPixels * heightPixels / 8; i++)
  {
    writeData(pgm_read_byte(&BW_image[i]));
  }

  //Aim back at the command register
  writeCMD(0x22);
  writeData(0xFF);
  writeCMD(0x20);
}



//================================================================================
//This function only works for displays <= 256 x 256 resolution
void partialTest(uint8_t pattern)
{
  uint8_t xStart, width, xEnd, yStart, height, yEnd;
  xStart = 160;
  yStart = 110;
  width = 24;
  height = 20;

  xStart = xStart / 8;
  width = width / 8;

  yEnd = yStart + height;
  xEnd = xStart + width - 1;


  //Make sure the display is not busy before starting a new command.
  WAIT_BUSY;

      //reset driver
  ePaper_RST_0;
  delay(10);
  ePaper_RST_1;
  delay(10);

  writeCMD(0x3c);
  writeData(0x80);

  writeCMD(0x44);
  writeData(xStart);
  writeData(xEnd);
  writeCMD(0x45);
  writeData(yStart);
  writeData(0x00);
  writeData(yEnd);
  writeData(0x00);


  writeCMD(0x4E);   // set RAM x address count to 0;
  writeData(xStart);
  writeCMD(0x4F);   // set RAM y address count to 0X127;    
  writeData(yStart);
  writeData(0x00);

  //Write the command: Write Ram (Black White)
  writeCMD(0x24);


  for (int i = 0; i < width * height; i++)
  {
    writeData(pattern);
  }

  //Aim back at the command register
  writeCMD(0x22);
  writeData(0xFF);
  writeCMD(0x20);
}


//================================================================================
void checkerboard()
{
  //Make sure the display is not busy before starting a new command.
  WAIT_BUSY;
  //Write the command: Write Ram (Black White)
  writeCMD(0x24);
  uint8_t pattern;
  pattern = 0x00;
  //rows
  for (int i = 0; i < VRES; i++)
  {
    if (i % 8 != 0)
      pattern = ~pattern;
    //columns = HRES / 8 bits since 1 bit per pixel... 8 pixels per byte sent
    for (int j = 0; j < HRES / 8; j++)
    {
      pattern = ~pattern;
      writeData(pattern);

    }
  }

  //Aim back at the command register
  writeCMD(0x22);
  writeData(0xF7);
  writeCMD(0x20);
}

//================================================================================
void solidColor(uint8_t pattern)
{
  //Make sure the display is not busy before starting a new command.
  WAIT_BUSY;
  //Write the command: Write Ram (Black White)
  writeCMD(0x24);
  //rows
  for (int i = 0; i < VRES; i++)
  {
    //columns = HRES / 8 bits since 1 bit per pixel... 8 pixels per byte sent
    for (int j = 0; j < HRES / 8; j++)
    {
      writeData(pattern);
    }
  }
  //Aim back at the command register
  writeCMD(0x22);
  writeData(0xF7);
  writeCMD(0x20);
}

//================================================================================
void solidColorBrush(uint8_t pattern)
{

  //Make sure the display is not busy before starting a new command.
  WAIT_BUSY;

  //Write the command: Write Ram (Black White)
  writeCMD(0x24);

  for (int i = 0; i < HRES * VRES / 8; i++)
  {
    writeData(pattern);
  }

  //Write the command: Write Ram (Black White)
  writeCMD(0x26);

  for (int i = 0; i < HRES * VRES / 8; i++)
  {
    writeData(pattern);
  }

  //Aim back at the command register
  writeCMD(0x22);
  writeData(0xF7);
  writeCMD(0x20);
}

//===========================================================================
void setup(void)
{
  //Debug port / Arduino Serial Monitor (optional)
  Serial.begin(115200);
  Serial.println("setup started");
  // Configure the pin directions   
  pinMode(EPD_CS, OUTPUT);
  pinMode(EPD_RESET, OUTPUT);
  pinMode(EPD_DC, OUTPUT);
  pinMode(EPD_READY, INPUT);
  pinMode(SD_CS, OUTPUT);
  pinMode(EPD_MOSI, OUTPUT);
  pinMode(EPD_SCK, OUTPUT);

  digitalWrite(SD_CS, LOW);

  //Set up SPI interface
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  SPI.begin();
  initEPD();
  Serial.println("setup complete");
}

//=============================================================================
#define waittime          5000
#define white             1
#define black             1
#define checks            1
#define splashscreen      1
#define partialupdate     1
void loop()
{
  Serial.println("top of loop");

#if white
  initEPD(); 
  solidColor(0x00);
  deepSleep();
  delay(waittime);
#endif

#if black
  initEPD();
  solidColor(0xff);
  deepSleep();
  delay(waittime);
#endif


#if checks
  initEPD();
  checkerboard();
  deepSleep();
  delay(waittime);
#endif

#if splashscreen
  initEPD();
  //load an image to the display
  loadFlashImage(HRES, VRES, Mono_1BPP);
  deepSleep();
  delay(waittime);
#endif

#if partialupdate
  initEPD();
  //load an image to the display

  loadFlashImageBrush(HRES, VRES, Mono_1BPP); //notice the "Brush" this is to put data in the "red color" GRAM
  delay(2000);
  partialTest(0xff);
  delay(500);
  partialTest(0x00);
  delay(500);
  partialTest(0xff);
  delay(500);
  partialTest(0x00);
  delay(500);
  partialTest(0xff);
  delay(500);
  partialTest(0x00);
  delay(500);
  loadPartialFlashImage(200, 100, 0, 100, Mono_1BPP_Partial);
  delay(waittime);

  //Go back to standard mode from partial update mode
  initEPD();
#endif

}
//=============================================================================
