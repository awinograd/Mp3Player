#include <SD.h>
#include <EEPROM.h>

#include <mp3.h>
#include <mp3conf.h>
#include <Song.h>
HardwareSerial Uart = HardwareSerial();

#define UART_BUFFER_SIZE 100
char val;         // variable to receive data from the Uart port
int ledpin = 11;  // LED connected to pin 2 (on-board LED)
Song song;
char response[100];

void setup()
{
  song.setup();
  song.nextFile();
  pinMode(ledpin, OUTPUT);  // pin 13 (on-board LED) as OUTPUT
  Uart.begin(9600);       // start Uart communication at 115200bps
}

void addKeyValuePair(char* response, const char* key, const char* val, boolean firstPair){
  char* appendChars = ",\"";
  int offset = 1;
  if (firstPair){
    strcpy(response, "{}");
    offset = 0;
    appendChars = "\"";
  }
  
  int len = strlen(response);
  int lenKey = strlen(key);
  int lenVal = strlen(val);
  strcpy(response+len-1, appendChars);
  strcpy(response+len+offset, key);
  strcpy(response+len+offset+lenKey, "\":\"");
  strcpy(response+len+offset+lenKey+3, val);
  strcpy(response+len+offset+lenKey+3+lenVal, "\"}");
  response[strlen(response)+1] = '\0';
}

void addKeyValuePair(char* response, const char* key, const char* val){
 addKeyValuePair(response, key, val, false); 
}

void returnPlayerState(){
  char buff[5];
  itoa(song.getVolume(), buff, 10);
  addKeyValuePair(response, "command", "CONNECTED", true);
  addKeyValuePair(response, "volume", buff);
  addKeyValuePair(response, "title", song.getCurrentSong());
  addKeyValuePair(response, "state", song.isPlaying() ? "PLAYING" : "PAUSED" );
  //addKeyValuePair(response, "title", buff);
  //addKeyValuePair(response, "volume2", "test");  
  Serial.println(response);
}

char* readCommand(char* buffer, char* data){
  boolean dataInfo = false;
  int i = 0;
  delay(UART_BUFFER_SIZE);
  
  while(Uart.available()){
    char inChar = Uart.read();
    if(!dataInfo){
      if (inChar == ','){
       i=0;
       dataInfo=true; 
       continue;
      }
      buffer[i] = inChar;  
      i++;
      if( i > UART_BUFFER_SIZE ){
        buffer[i-1] = '\0';
        Uart.print("Command too long.");
      }
      buffer[i] = '\0';
    }
    else{
      data[i] = inChar;
      i++;
      data[i] = '\0';
    }
  }
}

void loop() {  
  if( Uart.available() ) {      // if data is available to read
    char command[UART_BUFFER_SIZE];
    char data[10];
    readCommand(command, data);
    addKeyValuePair(response, "command", command, true);
    
    if (strcmp(command, "CONNECTED") == 0){
      returnPlayerState();
    }
    else if (strcmp(command, "LED")==0){
      int state = atoi(data);
      digitalWrite(ledpin, state);
      state == HIGH ? Uart.print("LIGHT ON") : Uart.print("LIGHT OFF");
    }
    else if (strcmp(command, "PLAY")==0){
      song.play();
      addKeyValuePair(response, "title", song.getCurrentSong());
    }
    else if (strcmp(command, "PAUSE")==0){
      song.pause();
      addKeyValuePair(response, "title", song.getCurrentSong());
    }
    else if (strcmp(command, "NEXT_TRACK")==0){
      boolean next = song.nextFile();
      if (next) {
        addKeyValuePair(response, "title", song.getCurrentSong());
      }
      else{
        addKeyValuePair(response, "message", "End of playlist");
      }
    }
    else if (strcmp(command, "PREV_TRACK")==0){
      boolean prev = song.prevFile();        
      if (prev) {
        addKeyValuePair(response, "title", song.getCurrentSong());
      }
      else{
        addKeyValuePair(response, "message", "Begining of playlist");
      }      
    }    
    else if (strcmp(command, "VOLUME") == 0) {
      int volume = atoi(data);
      double v = song.setVolume(volume);
    }
    else if (strcmp(command, "SEEK") == 0) {
      int seek = song.seek(atoi(data));
      int fs = song.getFileSize();
      if (seek){
       addKeyValuePair(response, "message", "1");
      }
      else{
               addKeyValuePair(response, "message", "0");
      }
    }
    else{
        addKeyValuePair(response, "command", "MESSAGE",true);
        addKeyValuePair(response, "message", "Command does not exist");
    }
    
    Serial.println(response);
    Uart.print(response);
    Uart.print('!');
  }
  song.loop();
}
