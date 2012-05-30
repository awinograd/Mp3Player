//#include <uart.h>
HardwareSerial Uart = HardwareSerial();

#define UART_BUFFER_SIZE 100
char val;         // variable to receive data from the Uart port
int ledpin = 11;  // LED connected to pin 2 (on-board LED)

void setup()
{
  pinMode(ledpin, OUTPUT);  // pin 13 (on-board LED) as OUTPUT
  Uart.begin(9600);       // start Uart communication at 115200bps
}

void respond(char* response){
  Uart.println(response);
  Serial.println(response); 
}

char* readCommand(char* buffer){
  int i = 0;
  delay(UART_BUFFER_SIZE);
  while(Uart.available()){
    char inChar = Uart.read();
    buffer[i] = inChar;  
    i++;
    if( i > UART_BUFFER_SIZE ){
      buffer[i-1] = '\0';
      respond("Command too long.");
    }
    buffer[i] = '\0';
}

void loop() {  
  if( Uart.available() ) {      // if data is available to read
    char command[UART_BUFFER_SIZE];
    readCommand(command);
    respond(command);
    if (strcmp(command, "HIGH")==0){
      digitalWrite(ledpin, HIGH);
      respond("LIGHT ON");
    }
    else if (strcmp(command, "LOW")==0){
      digitalWrite(ledpin, LOW);
      respond("LIGHT OFF");      
    }
    else if (strcmp(command, "PLAY")==0){
    }
    else if (strcmp(command, "PAUSE")==0){
    }
    else if (strcmp(command, "NEXT_TRACK")==0){
    }
    else if (strcmp(command, "PREVIOUS_TRACK")==0){
    }    
    else{
      respond("Command does not exist");     
    }
   
     command[0] = '\0'; 
  }

}
