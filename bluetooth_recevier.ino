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
  pinMode(ledpin, OUTPUT);  // pin 13 (on-board LED) as OUTPUT
  Uart.begin(9600);       // start Uart communication at 115200bps
}

void respond(char* response){
  Uart.println(response);
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
        respond("Command too long.");
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
    //respond(command);
    //respond(data);
    
    if (strcmp(command, "LED")==0){
      int state = atoi(data);
      digitalWrite(ledpin, state);
      state == HIGH ? respond("LIGHT ON") : respond("LIGHT OFF");
    }
    else if (strcmp(command, "PLAY")==0){
      song.play();
      respond("PLAYING");
    }
    else if (strcmp(command, "PAUSE")==0){
      song.pause();
      respond("PAUSED");      
    }
    else if (strcmp(command, "NEXT_TRACK")==0){
    }
    else if (strcmp(command, "PREVIOUS_TRACK")==0){
    }    
    else{
      respond("Command does not exist");     
    }
  }
  song.loop();
}
