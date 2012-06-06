#include <SD.h>
#include <EEPROM.h>

#include <mp3.h>
#include <mp3conf.h>
#include <Song.h>
#include <JsonHandler.h>

char val;         // variable to receive data from the Uart port
int ledpin = 11;  // LED connected to pin 2 (on-board LED)
Song song;
JsonHandler handler;

void setup()
{
  song.setup(&handler);
  song.nextFile();
  handler.setup();
  pinMode(ledpin, OUTPUT);  // pin 13 (on-board LED) as OUTPUT
}

void loop() {  
  if( handler.inputAvailable() ) {      // if data is available to read
    char command[120];
    char data[10];
    handler.readCommand(command, data);
    handler.addKeyValuePair("command", command, true);

    if (strcmp(command, "CONNECTED") == 0){
      song.sendPlayerState();
    }
    else if (strcmp(command, "LED")==0){
      int state = atoi(data);
      digitalWrite(ledpin, state);
      //state == HIGH ? Uart.print("LIGHT ON") : Uart.print("LIGHT OFF");
    }
    else if (strcmp(command, "PLAY")==0){
      song.play();
      song.sendSongInfo();
    }
    else if (strcmp(command, "PAUSE")==0){
      song.pause();
      song.sendSongInfo();
    }
    else if (strcmp(command, "NEXT_TRACK")==0){
      boolean next = song.nextFile();
      if (!next) {
        handler.addKeyValuePair("message", "End of playlist");
      }
    }
    else if (strcmp(command, "PREV_TRACK")==0){
      boolean prev = song.prevFile();        
      if (!prev) {
        handler.addKeyValuePair("message", "Begining of playlist");
      }      
    }    
    else if (strcmp(command, "VOLUME") == 0) {
      int volume = atoi(data);
      double v = song.setVolume(volume);
    }
    else if (strcmp(command, "SEEK") == 0) {
      int seek = song.seek(atoi(data));
      handler.addKeyValuePair("position", seek);
    }
    else{
      handler.addKeyValuePair("command", "MESSAGE",true);
      handler.addKeyValuePair("message", "Command does not exist");
    }

    handler.respond();
  }
  song.loop();
}

