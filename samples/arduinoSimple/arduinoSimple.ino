
#define COMMAND_BUFFER_SIZE 100
char commandBuffer[COMMAND_BUFFER_SIZE];
int commandBufferOffset;

void clearWorkbench();

void setup() { 
  Serial.begin(115200); 
  
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  
  commandBufferOffset = 0;
  commandBuffer[0] = '\0';
} 

void loop() {
  while(Serial.available()) {
    char ch = Serial.read();
    if(ch == '\r') {
    } else if(ch == '\n') {
      commandBuffer[commandBufferOffset] = '\0';
      if(commandBufferOffset == 0) {
        continue;
      } else if(!strcmp(commandBuffer, "!CONNECT")) {
        Serial.println("+OK");
        clearWorkbench();
      } else {
        Serial.println("-Invalid command");
      }
      commandBufferOffset = 0;
      commandBuffer[commandBufferOffset] = '\0';
    } else {
      commandBuffer[commandBufferOffset++] = ch;
      if(commandBufferOffset >= COMMAND_BUFFER_SIZE) {
        commandBufferOffset = 0;
      }
      commandBuffer[commandBufferOffset] = '\0';
    }
  }
  
  Serial.print("!main.data ");
  Serial.print(digitalRead(4) ? '1' : '0');
  Serial.print(',');
  Serial.print(digitalRead(5) ? '1' : '0');
  Serial.print(',');
  Serial.print(digitalRead(6) ? '1' : '0');
  Serial.print(',');
  Serial.print(digitalRead(7) ? '1' : '0');
  Serial.print(',');
  Serial.print(analogRead(0));
  Serial.println();
  
  delay(100);
}

void clearWorkbench() {
  Serial.println("!clear");
  Serial.println("!set name,ArduinoSimple"); 
  Serial.println("!set description,'Simple Arduino sketch to read and write analog and digial'"); 

  Serial.println("?create plot");
  Serial.println("!add graph,main,0,0,1,1");

  Serial.println("?add signals");
  Serial.println("!main.set timePerSample,0.000008");
  Serial.println("!main.addSignal d2,1,0,5");
  Serial.println("!main.addSignal d3,1,0,5");
  Serial.println("!main.addSignal d4,1,0,5");
  Serial.println("!main.addSignal d5,1,0,5");
  Serial.println("!main.addSignal a0,10,0,5");
}
