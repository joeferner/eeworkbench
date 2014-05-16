
#include <stdint.h>

//#define TIME_PER_SAMPLE   0.000002
#define TIME_PER_SAMPLE   0.00002

#define DEBUG_PIN         2
#define TICKS_PER_SAMPLE  (F_CPU * TIME_PER_SAMPLE)

#define BUFFER_SIZE 512
char buffer[BUFFER_SIZE];
uint16_t bufferOffset;

#define COMMAND_BUFFER_SIZE 100
char commandBuffer[COMMAND_BUFFER_SIZE];
int commandBufferOffset;

boolean triggered;

void clearWorkbench();
void disableTimer1Interrupt();
void enableTimer1Interrupt();
boolean isTimer1Enabled();
boolean trigger();
void writeBufferData();

boolean trigger() {
  return digitalRead(13);
}

void setup() { 
  Serial.begin(115200); 
  commandBufferOffset = 0;
  commandBuffer[0] = '\0';

  pinMode(DEBUG_PIN, OUTPUT);
  pinMode(8, INPUT);  // PB0
  pinMode(9, INPUT);  // PB1
  pinMode(10, INPUT); // PB2
  pinMode(11, INPUT); // PB3
  pinMode(12, INPUT); // PB4
  pinMode(13, INPUT); // PB5

  cli();

//  TIMSK0 = 0; // disable timer0 interrupt

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  TCCR1B |= (1 << WGM12); // CTC mode

  if(TICKS_PER_SAMPLE < 0xffff) {
    OCR1A = TICKS_PER_SAMPLE;
    TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10); // 1 prescaler
  } else if((TICKS_PER_SAMPLE / 8) < 0xffff) {
    OCR1A = TICKS_PER_SAMPLE / 8;
    TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10); // 8 prescaler
  } else if((TICKS_PER_SAMPLE / 64) < 0xffff) {
    OCR1A = TICKS_PER_SAMPLE / 64;
    TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10); // 64 prescaler
  } else if((TICKS_PER_SAMPLE / 256) < 0xffff) {
    OCR1A = TICKS_PER_SAMPLE / 256;
    TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10); // 256 prescaler
  } else {
    OCR1A = TICKS_PER_SAMPLE / 1024;
    TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10); // 1024 prescaler
  }
  
  disableTimer1Interrupt();

  bufferOffset = 0;
  triggered = false;
  
  sei();
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
  
  boolean t = trigger();
  if(triggered && !t) {
    triggered = false;
  }
  
  if(!isTimer1Enabled() && t && !triggered) {
    bufferOffset = 0;
    triggered = true;
    enableTimer1Interrupt();
    while(bufferOffset < BUFFER_SIZE) {
      digitalWrite(DEBUG_PIN, digitalRead(DEBUG_PIN) ^ 1);
    }
    digitalWrite(DEBUG_PIN, 0);
    writeBufferData();
  }
}

boolean isTimer1Enabled() {
  return (TIMSK1 & (1 << OCIE1A));
}

void disableTimer1Interrupt() {
  TIMSK1 &= ~(1 << OCIE1A);
}

void enableTimer1Interrupt() {
  TCNT1 = 0;
  TIMSK1 |= (1 << OCIE1A);
}

void writeBufferData() {
  Serial.print("!main.beginData ");
  Serial.println(BUFFER_SIZE);
  for(int i = 0; i < BUFFER_SIZE; i++) {
    Serial.print(buffer[i]);
  }
  Serial.println();
}

void clearWorkbench() {
  Serial.println("!clear");
  Serial.println("!set name,'Arduino Logic Analyzer'"); 
  Serial.println("!set description,'6-bit Logic Analyzer'"); 

  Serial.println("?create plot");
  Serial.println("!add graph,main,0,0,1,1");

  Serial.println("?add signals");
  Serial.print("!main.set timePerSample,");
  Serial.println(TIME_PER_SAMPLE, 10);
  Serial.println("!main.addSignal d5,1,0,5");
  Serial.println("!main.addSignal d4,1,0,5");
  Serial.println("!main.addSignal d3,1,0,5");
  Serial.println("!main.addSignal d2,1,0,5");
  Serial.println("!main.addSignal d1,1,0,5");
  Serial.println("!main.addSignal d0,1,0,5");

  Serial.print("?ticksPerSample: ");
  Serial.println(TICKS_PER_SAMPLE);
  Serial.print("?timePerSample: ");
  Serial.println(TIME_PER_SAMPLE, 10);
}

ISR(TIMER1_COMPA_vect) {
  buffer[bufferOffset++] = PORTB << 2;
  if(bufferOffset == BUFFER_SIZE) {
    disableTimer1Interrupt();
  }
}

