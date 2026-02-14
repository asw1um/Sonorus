#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

// -------- VS1053 Pins --------
#define VS1053_RESET 9
#define VS1053_CS    10
#define VS1053_DCS   8
#define VS1053_DREQ  3
#define SDCS         4

// -------- Button + Pot Pins --------
#define BUTTON_PLAY   6
#define BUTTON_PREV   5
#define BUTTON_NEXT 7
#define POT_PIN       A0

Adafruit_VS1053_FilePlayer musicPlayer(
  VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, SDCS);

// -------- Button States --------
bool lastPlayState = HIGH;
bool lastStopState = HIGH;
bool lastRestartState = HIGH;

void setup() {
  Serial.begin(9600);
  

  // Buttons with internal pullups
  pinMode(BUTTON_PLAY, INPUT_PULLUP);
  pinMode(BUTTON_STOP, INPUT_PULLUP);
  pinMode(BUTTON_RESTART, INPUT_PULLUP);

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

  // -------- Keep Feeding Audio --------
  if (musicPlayer.playingMusic) {
    musicPlayer.feedBuffer();
  }

  // -------- Read Potentiometer (Volume Control) --------
  int potValue = analogRead(POT_PIN);

  // Map 0-1023 to 0-100 (VS1053 volume scale, lower = louder)
  int volume = map(potValue, 0, 1023, 100, 0);
  musicPlayer.setVolume(volume, volume);

  // -------- Read Buttons --------
  bool playState = digitalRead(BUTTON_PLAY);
  bool stopState = digitalRead(BUTTON_STOP);
  bool restartState = digitalRead(BUTTON_RESTART);

  // Play / Pause Toggle
  if (playState == LOW && lastPlayState == HIGH) {
    if (musicPlayer.playingMusic) {
      musicPlayer.pausePlaying(true);
      Serial.println("PAUSED");
    } else {
      musicPlayer.pausePlaying(false);
      Serial.println("RESUMED");
    }
    delay(200); // simple debounce
  }
  lastPlayState = playState;

  // Stop
  if (stopState == LOW && lastStopState == HIGH) {
    musicPlayer.stopPlaying();
    Serial.println("STOPPED");
    delay(200);
  }
  lastStopState = stopState;

  // Restart
  if (restartState == LOW && lastRestartState == HIGH) {
    musicPlayer.stopPlaying();
    musicPlayer.startPlayingFile("test.mp3");
    Serial.println("RESTARTED");
    delay(200);
  }
  lastRestartState = restartState;
}
