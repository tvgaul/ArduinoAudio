#include <Arduino.h>
#include<math.h>
#include <SPI.h>
#include <SD.h>

#define SD_CS 10
#define BUFFERSIZE 256
uint8_t buffer[2][BUFFERSIZE];
volatile uint8_t readInd;
volatile uint8_t sampleReadInd;
uint32_t sampleCount;
uint32_t currentSample = 0;
uint8_t lastInd;
File file;
void setup() {

  pinMode(3, OUTPUT);
  TCCR2A = _BV(COM2A0) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20);
  OCR2A = 255;
  OCR2B = 5;

 // Configure Timer1 for CTC (Clear Timer on Compare Match) mode
  // Configure Timer1 for 8 kHz interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= _BV(WGM12); // CTC mode
  TCCR1B |= _BV(CS10);  // Prescaler 1
  OCR1A = 1999;          // 8 kHz interrupt rate
  TIMSK1 |= _BV(OCIE1A); // Enable Timer1 interrupt

    if (!SD.begin(SD_CS)) {
        Serial.begin(9600);
        Serial.println("SD Card initialization failed!");
        return;
    }
    
    file = SD.open("virus.wav");
    
    if (!file) {
        Serial.begin(9600);
        Serial.println("Failed to open test.wav");
        return;
    }

    // Serial.println("Reading WAV file:");
    file.read(buffer[0],44);
    // Serial.print("Channels: ");
    // Serial.println((buffer[0][23]<<8)|buffer[0][22]);
    // Serial.print("Sample Width: ");
    // Serial.print(((buffer[0][35]<<8)|buffer[0][34])/8);
    // Serial.println(" bytes");
    // Serial.print("Frame Rate (Sample Rate): ");
    // Serial.print(((uint32_t)buffer[0][27]<<24)|((uint32_t)buffer[0][26]<<16)|((uint32_t)buffer[0][25]<<8)|(uint32_t)buffer[0][24]); 
    // Serial.println(" Hz");
    // Serial.print("Total Frames: ");
    sampleCount = ((uint32_t)buffer[0][43] << 24) | ((uint32_t)buffer[0][42] << 16) | ((uint32_t)buffer[0][41] << 8) | (uint32_t)buffer[0][40];
    // Serial.println(((uint32_t)buffer[0][43] << 24) | ((uint32_t)buffer[0][42] << 16) | ((uint32_t)buffer[0][41] << 8) | (uint32_t)buffer[0][40]); 
    // Serial.print("Duration: ");
    // Serial.print((((uint32_t)buffer[0][43] << 24) | ((uint32_t)buffer[0][42] << 16) | ((uint32_t)buffer[0][41] << 8) | (uint32_t)buffer[0][40])/(((uint32_t)buffer[0][27]<<24)|((uint32_t)buffer[0][26]<<16)|((uint32_t)buffer[0][25]<<8)|(uint32_t)buffer[0][24]));
    // Serial.println(" seconds");
    // Serial.print("First 10 bytes of audio data: ");
    // file.read(buffer[0],44);
    // for(int i = 0;i<10;i++){
    //   Serial.print(buffer[0][i],HEX);
    // }
    file.read(buffer[0],BUFFERSIZE);
    file.read(buffer[1],BUFFERSIZE);
    readInd=0;
    lastInd = 0;
    sampleReadInd = 0;
    sei();  // Enable global interrupts
    

    

}

ISR(TIMER1_COMPA_vect) {
  OCR2B = buffer[readInd][sampleReadInd];  // Output current sample

  sampleReadInd = (sampleReadInd + 1) % BUFFERSIZE; // Increment sample index first

  if (sampleReadInd == 0) {  // Only switch buffers when a full buffer is read
      readInd = (readInd + 1) % 2;
  }
}

void end(){
  file.close();
  while(1){
    if(readInd!=lastInd){
      OCR2B = 0;
      cli();
      while(1);
    }
  }

}

void loop(){
  if(lastInd != readInd){
    file.read(buffer[1-readInd],min(sampleCount-currentSample,BUFFERSIZE));
    currentSample +=min(sampleCount-currentSample,BUFFERSIZE);
  }
  lastInd = readInd;
  if(currentSample>=sampleCount){
    end();
  }
}