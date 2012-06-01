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

void setup()
{
  song.setup();
  song.nextFile();
  pinMode(ledpin, OUTPUT);  // pin 13 (on-board LED) as OUTPUT
  Uart.begin(9600);       // start Uart communication at 115200bps
}

void addKeyValuePair(char* response, char* key, char* val, boolean firstPair){
  char* appendChars = ",\"";
  int offset = 1;
  if (firstPair){
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

void addKeyValuePair(char* response, char* key, char* val){
 addKeyValuePair(response, key, val, false); 
}

void returnPlayerState(){
  char response[100];
  strcpy(response, "{}");
  //strcpy(response, "{\"volume\": ");
  //itoa(song.getVolume(), response+strlen(response), 10)
  //strcpy(response+strlen(response), "}\0");
  char buff[5];
  itoa(song.getVolume(), buff, 10);
  addKeyValuePair(response, "msg", "CONNECTED");
  addKeyValuePair(response, "volume", buff, true);
  addKeyValuePair(response, "volume2", "test");  
  Uart.print(response);
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
    //Uart.print(command);
    //Uart.print(data);
    
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
      Uart.print("sPLAYING");
    }
    else if (strcmp(command, "PAUSE")==0){
      song.pause();
      Uart.print("PAUSED");      
    }
    else if (strcmp(command, "NEXT_TRACK")==0){
      boolean next = song.nextFile();
      if (next) {
        Uart.print(song.getCurrentSong()); 
      }
      else{
        Uart.print("END OF SONGS");
      }
    }
    else if (strcmp(command, "PREV_TRACK")==0){
      song.prevFile();      
      Uart.print("PREV");     
    }    
    else if (strcmp(command, "VOLUME") == 0) {
      int volume = atoi(data);
      double v = song.setVolume(volume);
      Uart.print("VOLUME: ");
      Uart.print(v);
      Uart.print("!");
    }
    else if (strcmp(command, "SEEK") == 0) {
      int seek = song.seek(atoi(data));
      int fs = song.getFileSize();
      Uart.println(atoi(data)*fs);
      Uart.println(atoi(data));
      Uart.print("ADF");
      Uart.println(seek);
      Uart.print("SEEKING");
    }
    else{
      Uart.print("Command does not exist");     
    }
    
    Uart.print('!');
  }
  song.loop();
}
