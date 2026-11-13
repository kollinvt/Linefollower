/*
 * EEPROMAnything - schrijf / lees eender welk datatype (ook een struct)
 * in één keer naar / uit het EEPROM geheugen.
 *
 * EEPROM.update() schrijft enkel bytes die effectief veranderd zijn,
 * zo sparen we de levensduur van het EEPROM (+- 100 000 schrijfcycli).
 */
#ifndef EEPROMANYTHING_H
#define EEPROMANYTHING_H

#include <Arduino.h>
#include <EEPROM.h>

template <class T>
int EEPROM_writeAnything(int address, const T &value)
{
  const byte *p = (const byte *)(const void *)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++) EEPROM.update(address++, *p++);
  return i;
}

template <class T>
int EEPROM_readAnything(int address, T &value)
{
  byte *p = (byte *)(void *)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++) *p++ = EEPROM.read(address++);
  return i;
}

#endif
