/*
 * SerialCommand - non-blocking commando parser voor eender welke Stream
 * (Serial, Serial1, SoftwareSerial, ...).
 *
 * Gebaseerd op het idee van de SerialCommand bibliotheek van Steven Cogswell,
 * herschreven zodat meerdere poorten (USB + Bluetooth) tegelijk kunnen
 * luisteren en dezelfde eventhandlers kunnen delen.
 *
 * Een commando ziet er zo uit:
 *   <commandowoord> <spatie> <argument 1> <spatie> <argument 2> <CR+LF>
 *
 * In een eventhandler haal je de argumenten op met sCmd.next() of, wanneer
 * meerdere SerialCommand objecten dezelfde handler gebruiken, met
 * SerialCommand::current->next(). Antwoorden stuur je naar
 * SerialCommand::current->port() zodat het antwoord teruggaat naar de poort
 * waarop het commando binnenkwam.
 */
#ifndef SERIALCOMMAND_H
#define SERIALCOMMAND_H

#include <Arduino.h>

#define SERIALCOMMAND_BUFFER 40   // max. lengte van 1 commando (incl. argumenten)
#define SERIALCOMMAND_MAXCMDS 12  // max. aantal commandowoorden
#define SERIALCOMMAND_WORDLEN 10  // max. lengte van een commandowoord

class SerialCommand
{
public:
  explicit SerialCommand(Stream &stream);

  void addCommand(const char *command, void (*function)());
  void setDefaultHandler(void (*function)(char *));

  void readSerial();     // non-blocking: verwerkt enkel de karakters die al binnen zijn
  void clearBuffer();
  char *next();          // volgend argument, of NULL wanneer er geen meer is
  Stream &port() { return stream; }

  static SerialCommand *current; // object dat op dit moment een commando afhandelt

private:
  struct Command
  {
    char word[SERIALCOMMAND_WORDLEN + 1];
    void (*function)();
  };

  Stream &stream;
  Command commands[SERIALCOMMAND_MAXCMDS];
  byte commandCount;
  void (*defaultHandler)(char *);

  char buffer[SERIALCOMMAND_BUFFER + 1];
  byte bufPos;
  char *last; // strtok_r state
};

#endif
