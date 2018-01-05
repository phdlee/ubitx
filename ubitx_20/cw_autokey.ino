/*************************************************************************
  This source code is written for uBITX, but it can also be used on other radios.
  To minimize the use of memory space, we used bitwise operations.
  For the alphabet, I put Morsecode in 1 byte. The front 4Bit is the length and the 4Bit is the Morse code.
  Because the number is fixed in length, there is no separate length information. The 5Bit on the right side is the Morse code.

  If you exclude the '$' symbol, can put Length information to cwSymbolTable like Alphabet. Currently, it consumes 18 bytes to store Length.
  by KD8CEC  
-----------------------------------------------------------------------------
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************************/
#include <avr/pgmspace.h>

//27 + 10 + 18 + 1(SPACE) = //56 
const PROGMEM uint8_t cwAZTable[27] = {0b00100100 , 0b01001000 , 0b01001010 , 0b00111000 , 0b00010000, 0b01000010, 0b00111100, 0b01000000 , //A ~ H
0b00100000, 0b01000111 ,0b00111010, 0b01000100, 0b00101100, 0b00101000 , 0b00111110, 0b01000110, 0b01001101, 0b00110100, //I ~ R
0b00110000, 0b00011000, 0b00110010, 0b01000001, 0b00110110, 0b01001001, 0b01001011, 0b00111000};  //S ~ Z
PGM_P pCwAZTable = reinterpret_cast<PGM_P>(cwAZTable);

const PROGMEM uint8_t cw09Table[27] = {0b00011111, 0b00001111, 0b00000111, 0b00000011, 0b00000001, 0b00000000, 0b00010000, 0b00011000, 0b00011100, 0b00011110};
PGM_P pcw09Table = reinterpret_cast<PGM_P>(cw09Table);

const PROGMEM uint8_t cwSymbolIndex[] =  {'.',         ',',        '?',         '"',       '!',         '/',      '(',       ')',        '&',        ':',        ';',         '=',        '+',        '-',        '_',        '\'',       '@' };
PGM_P pCwSymbolIndex = reinterpret_cast<PGM_P>(cwSymbolIndex);

const PROGMEM uint8_t cwSymbolTable[]  = {0b11010101, 0b11110011, 0b11001100, 0b11011110, 0b11101011, 0b10100100, 0b10101100, 0b11101101, 0b10010000, 0b11111000, 0b11101010, 0b10100010, 0b10010100, 0b11100001, 0b11001101, 0b11010010,  0b11011010};
PGM_P pCwSymbolTable = reinterpret_cast<PGM_P>(cwSymbolTable);
////const PROGMEM uint8_t cwSymbolLength[] = {6,          6,          6,         6,           6,          5,          5,          6,          5,          6,          6,          5,          5,          6,          6,          6,         6};

void sendCWChar(char cwKeyChar)
{
  byte sendBuff[7];
  byte i, j, charLength;
  byte tmpChar;

  //Encode Char by KD8CEC
  if (cwKeyChar >= 'A' && cwKeyChar <= 'Z')
  {
    tmpChar = pgm_read_byte(pCwAZTable + (cwKeyChar - 'A'));
    charLength = (tmpChar >> 4) & 0x0F;
    for (i = 0; i < charLength; i++)
      sendBuff[i] = (tmpChar << i) & 0x08;
  }
  else if (cwKeyChar >= '0' && cwKeyChar <= '9')
  {
    charLength = 5;
    for (i = 0; i < charLength; i++)
      sendBuff[i] = (pgm_read_byte(pcw09Table + (cwKeyChar - '0')) << i) & 0x10;
  }
  else if (cwKeyChar == ' ')
  {
    delay_background(cwSpeed * 7, 3);
  }
  else if (cwKeyChar == '$')  //7 digit
  {
    charLength = 7;
    for (i = 0; i < 7; i++)
      sendBuff[i] = (0b00010010 << i) & 0x80; //...1..1
  }
  else
  {
    //symbol
    for (i = 0; i < 17; i++)
    {
      if (pgm_read_byte(pCwSymbolIndex + i) == cwKeyChar)
      {
        tmpChar = pgm_read_byte(pCwSymbolTable + i);
        charLength = ((tmpChar >> 6) & 0x03) + 3;
        
        for (j = 0; j < charLength; j++)
          sendBuff[j] = (tmpChar << j + 2) & 0x80;

        break;
      }
    }
  }

  for (i = 0; i < charLength; i++)
  {
    cwKeydown();
    if (sendBuff[i] == 0)
      delay_background(cwSpeed, 3);
    else
      delay_background(cwSpeed * 3, 3);
    cwKeyUp();
    if (i != charLength -1)
      delay_background(cwSpeed, 3);
  }
}

void sendAutoCW(int cwSendLength, char *sendString)
{
  byte i;

  if (!inTx){
    keyDown = 0;
    cwTimeout = millis() + cwDelayTime * 10;
    startTx(TX_CW, 0);  //disable updateDisplay Command for reduce latency time
    updateDisplay();
  }

  for (i = 0; i < cwSendLength; i++)
  {
    sendCWChar(sendString[i]);
    if (i != cwSendLength -1) delay_background(cwSpeed * 3, 3);
  }

  delay_background(cwDelayTime * 10, 2);
  stopTx();
}

