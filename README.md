# CFAP200200Ax-0154 Demonstration Code

This is Arduino sample code for the CFAP200200Ax-0154 family of displays. These displays are 2-color ePapers and use the [SSD1681](https://www.crystalfontz.com/controllers/SolomonSystech/SSD1681/) LCD controller. This family of displays operate on SPI. For quick and easy interfacing with the display, the [CFA100084 ePaper Adapter Board](https://www.crystalfontz.com/product/cfa10084-epaper-adapter-board) can be used.

## Connection Guide
```
//  ARD      | Port  | Adapter pin |  Function - SPI                          |
//-----------+-------+-------------+------------------------------------------+
// 3.3V      |       |  01         |  POWER 3.3V                              |
// GND       |       |  02         |  GROUND                                  |
//-----------+-------+-------------+------------------------------------------+
// D3        | PORTD |  08         |  Busy                            (BSY)   |
// D4        | PORTD |  07         |  Reset                           (RST)   |
// D5        | PORTD |  06         |  Data/Command                    (DC)    |
// D10       | PORTB |  05         |  Chip Select                     (CS)    |
// D11       | PORTB |  04         |  Master Out Slave In             (MOSI)  |
// D13       | PORTB |  03         |  Serial Clock                    (SCK)   |
//-----------+-------+-------------+------------------------------------------+
```

## Display Information
Here are links to our active displays:\
[CFAP200200A3-0154](https://www.crystalfontz.com/product/cfap200200a30154)

For more inforamtion about other ePaper offerings, please see our full list [here](https://www.crystalfontz.com/c/epaper-displays/519).