#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

#define VS1053_RESET 9
#define VS1053_CS    10
#define VS1053_DCS   8
#define VS1053_DREQ  3
#define SDCS         4

Adafruit_VS1053_FilePlayer musicPlayer(
  VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, SDCS);

void setup() {
  Serial.begin(9600);
  delay(2000);

  if (!musicPlayer.begin()) {
    Serial.println("VS1053 NOT FOUND");
    while (1);
  }
  Serial.println("VS1053 FOUND");

  if (!SD.begin(SDCS)) {
    Serial.println("SD FAIL");
    while (1);
  }
  Serial.println("SD OK");

  musicPlayer.setVolume(20, 20);

  if (!musicPlayer.startPlayingFile("test.mp3")) {
    Serial.println("FILE NOT FOUND");
    while (1);
  }
}

void loop() {
  // Must keep feeding the buffer while playing
  if (musicPlayer.playingMusic) {
    musicPlayer.feedBuffer();
    Serial.println("PLAYING");
  } else {
    Serial.println("NOT PLAYING");
  }
  
}
