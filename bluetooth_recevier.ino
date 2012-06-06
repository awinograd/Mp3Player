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
  song.setup();
  song.nextFile();
  handler.setup();
  pinMode(ledpin, OUTPUT);  // pin 13 (on-board LED) as OUTPUT
}


void returnPlayerState(){
  char buff[5];
  itoa(song.getVolume(), buff, 10);
  handler.addKeyValuePair("command", "CONNECTED", true);
  handler.addKeyValuePair("volume", buff);
  addSongInfoToResponse();
}

void addSongInfoToResponse(){
  handler.addKeyValuePair("title", song.getTitle());
  handler.addKeyValuePair("artist", song.getArtist());
  handler.addKeyValuePair("album", song.getAlbum());
  handler.addKeyValuePair("time", song.getTime());
  handler.addKeyValuePair("state", song.isPlaying() ? "PLAYING" : "PAUSED" );
}

void loop() {  
  if( handler.inputAvailable() ) {      // if data is available to read
    char command[120];
    char data[10];
    handler.readCommand(command, data);
    handler.addKeyValuePair("command", command, true);

    if (strcmp(command, "CONNECTED") == 0){
      returnPlayerState();
    }
    else if (strcmp(command, "LED")==0){
      int state = atoi(data);
      digitalWrite(ledpin, state);
      //state == HIGH ? Uart.print("LIGHT ON") : Uart.print("LIGHT OFF");
    }
    else if (strcmp(command, "PLAY")==0){
      Serial.println("PLAY----");
      song.play();
      addSongInfoToResponse();
    }
    else if (strcmp(command, "PAUSE")==0){
      song.pause();
      addSongInfoToResponse();
    }
    else if (strcmp(command, "NEXT_TRACK")==0){
      boolean next = song.nextFile();
      if (next) {
        addSongInfoToResponse();
      }
      else{
        handler.addKeyValuePair("message", "End of playlist");
      }
    }
    else if (strcmp(command, "PREV_TRACK")==0){
      boolean prev = song.prevFile();        
      if (prev) {
        addSongInfoToResponse();
      }
      else{
        handler.addKeyValuePair("message", "Begining of playlist");
      }      
    }    
    else if (strcmp(command, "VOLUME") == 0) {
      int volume = atoi(data);
      double v = song.setVolume(volume);
    }
    else if (strcmp(command, "SEEK") == 0) {
      song.sendPlayerState();
      int seek = song.seek(atoi(data));
      int fs = song.getFileSize();
      if (seek){
        handler.addKeyValuePair("message", "1");
      }
      else{
        handler.addKeyValuePair("message", "0");
      }
    }
    else{
      handler.addKeyValuePair("command", "MESSAGE",true);
      handler.addKeyValuePair("message", "Command does not exist");
    }

    handler.respond();
  }
  song.loop();
}

