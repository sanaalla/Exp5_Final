//Experiment 5 - Multiples and Portables
//"FlattEar Me"
//DIGF-6037-001 Creation & Computation
//Digital Futures MDes, Dec. 4 2017
//Sana Shepko, Ramona Caprariu, Kylie Caraway

//Code based on Adafruit example for Music Maker featherwing. 

//A portable comforting device, worn on the user's head. When feeling distressed or looking for reassurance, the user presses the button behind their ear and a compliment will play. 

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

//List the names of the compliment audio files, so that they can be treated as a character array here and randomized on button press
char *tracklist[] = {"track001.mp3", "track002.mp3", "track003.mp3", "track004.mp3", "track005.mp3", "track006.mp3", "track007.mp3", "track008.mp3", "track009.mp3", "track010.mp3"};


// These are the pins used
#define VS1053_RESET   -1     // VS1053 reset pin (not used!)

// Feather M0 or 32u4
#if defined(__AVR__) || defined(ARDUINO_SAMD_FEATHER_M0)
  #define VS1053_CS       6     // VS1053 chip select pin (output)
  #define VS1053_DCS     10     // VS1053 Data/command select pin (output)
  #define CARDCS          5     // Card chip select pin
  // DREQ should be an Int pin *if possible* (not possible on 32u4)
  #define VS1053_DREQ     9     // VS1053 Data request, ideally an Interrupt pin
#endif


int buttonPin = 13;
int buttonPrev = 1;
int buttonVal;

Adafruit_VS1053_FilePlayer musicPlayer = 
Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT);
  Serial.println(buttonPin);
  
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }

  Serial.println(F("VS1053 found"));
   
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
  // list files
  printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(0,0);
  
#if defined(__AVR_ATmega32U4__) 
  // Timer interrupts are not suggested, better to use DREQ interrupt!
  // but we don't have them on the 32u4 feather...
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int
#elif defined(ESP32)
  // no IRQ! doesn't work yet :/
#else
  // If DREQ is on an interrupt pin we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
#endif
  
}

void loop() {
//Read the button pin
buttonVal = digitalRead(buttonPin);  

//If button changes state, then play a random compliment
if((buttonVal==1)&&(buttonPrev==0)) {
Serial.println(F("Playing full track"));
  int compliment = random(0,10);
  musicPlayer.playFullFile(tracklist[compliment]);
}

//See the button value, and if it's changed state
Serial.println(buttonVal);

//  delay(100);
buttonPrev = buttonVal; //store the value of this cycle to compare next loop

  
}



/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}


