#include "SerialCommand.h"
#include <string.h>

SerialCommand *SerialCommand::current = NULL;

static const char DELIMITERS[] = " ";

SerialCommand::SerialCommand(Stream &s)
    : stream(s), commandCount(0), defaultHandler(NULL), bufPos(0), last(NULL)
{
  clearBuffer();
}

void SerialCommand::addCommand(const char *command, void (*function)())
{
  if (commandCount >= SERIALCOMMAND_MAXCMDS) return;
  strncpy(commands[commandCount].word, command, SERIALCOMMAND_WORDLEN);
  commands[commandCount].word[SERIALCOMMAND_WORDLEN] = '\0';
  commands[commandCount].function = function;
  commandCount++;
}

void SerialCommand::setDefaultHandler(void (*function)(char *))
{
  defaultHandler = function;
}

void SerialCommand::clearBuffer()
{
  buffer[0] = '\0';
  bufPos = 0;
}

char *SerialCommand::next()
{
  return strtok_r(NULL, DELIMITERS, &last);
}

void SerialCommand::readSerial()
{
  // enkel verwerken wat al in de ontvangstbuffer zit => blokkeert nooit
  while (stream.available() > 0)
  {
    char c = stream.read();

    if (c == '\r') continue; // CR negeren, LF sluit het commando af

    if (c == '\n')
    {
      if (bufPos == 0) continue; // lege lijn

      char *word = strtok_r(buffer, DELIMITERS, &last);
      if (word != NULL)
      {
        current = this;
        bool matched = false;
        for (byte i = 0; i < commandCount; i++)
        {
          if (strcmp(word, commands[i].word) == 0)
          {
            commands[i].function();
            matched = true;
            break;
          }
        }
        if (!matched && defaultHandler != NULL) defaultHandler(word);
      }
      clearBuffer();
    }
    else if (isprint(c))
    {
      if (bufPos < SERIALCOMMAND_BUFFER)
      {
        buffer[bufPos++] = c;
        buffer[bufPos] = '\0';
      }
      // te lange commando's worden afgekapt
    }
  }
}
