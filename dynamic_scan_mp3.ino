#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>


#define DEBUG 0  // enable output (increases memory a lot)

// pins
#define VS1053_RESET 9
#define VS1053_CS    10
#define VS1053_DCS   8
#define VS1053_DREQ  3
#define SDCS         4


#define BUTTON_PLAY  5
#define BUTTON_PREV  6
#define BUTTON_NEXT  7
#define POT_PIN      A0

#define DEBOUNCE_TIME 50  //ms

Adafruit_VS1053_FilePlayer musicPlayer(
  VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, SDCS);

// debounce
struct Button {
  int pin;
  bool lastReading;
  bool state;
  unsigned long lastDebounceTime;
};

Button playBtn = {BUTTON_PLAY, HIGH, HIGH, 0};
Button prevBtn = {BUTTON_PREV, HIGH, HIGH, 0};
Button nextBtn = {BUTTON_NEXT, HIGH, HIGH, 0};

// state
bool isPaused = false;

// Track node
#define MAX_FILENAME 13
int totalTracks = 0;
int currentTrack = 0;

char currentTrackFile[MAX_FILENAME] = {0};
char nextTrackFile[MAX_FILENAME] = {0};
char prevTrackFile[MAX_FILENAME] = {0};

// sd scanner
void scanSD() {
  File root = SD.open("/");
  totalTracks = 0;

  if (DEBUG) Serial.println("Scanning SD...");

  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;

    if (!entry.isDirectory()) {
      String name = entry.name();
      name.toLowerCase();
      if (name.endsWith(".mp3")) {
        totalTracks++;
        if (DEBUG) {
          Serial.print("Found: ");
          Serial.println(name);
        }
      }
    }
    entry.close();
  }
  root.close();

  if (totalTracks == 0) {
    if (DEBUG) Serial.println("No MP3 files found!");
    while (1);
  }

  if (DEBUG) {
    Serial.print("Total tracks: ");
    Serial.println(totalTracks);
  }
}

// get filename by index 
void getTrackFilename(int index, char* buf) {
  int count = 0;
  File root = SD.open("/");

  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;

    if (!entry.isDirectory()) {
      String name = entry.name();
      name.toLowerCase();
      if (name.endsWith(".mp3")) {
        if (count == index) {
          strncpy(buf, name.c_str(), MAX_FILENAME - 1);
          buf[MAX_FILENAME - 1] = '\0';
          entry.close();
          root.close();
          return;
        }
        count++;
      }
    }
    entry.close();
  }

  root.close();
  buf[0] = '\0';
}


void updateTrackFiles() {
  getTrackFilename(currentTrack, currentTrackFile);
  getTrackFilename((currentTrack + 1) % totalTracks, nextTrackFile);
  getTrackFilename((currentTrack - 1 + totalTracks) % totalTracks, prevTrackFile);
}


void playCurrentTrack() {
  if (DEBUG) {
    Serial.print("Playing ");
    Serial.println(currentTrackFile);
  }

  musicPlayer.stopPlaying();
  musicPlayer.startPlayingFile(currentTrackFile);
  isPaused = false;
}


bool checkButton(Button &btn) {
  bool reading = digitalRead(btn.pin);

  if (reading != btn.lastReading) {
    btn.lastDebounceTime = millis();
  }

  if ((millis() - btn.lastDebounceTime) > DEBOUNCE_TIME) {
    if (reading != btn.state) {
      btn.state = reading;
      if (btn.state == LOW) {
        btn.lastReading = reading;
        return true;  
      }
    }
  }

  btn.lastReading = reading;
  return false;
}

void setup() {
  if (DEBUG) Serial.begin(9600);
  delay(1500);

  pinMode(10, OUTPUT); // SPI master
  pinMode(BUTTON_PLAY, INPUT_PULLUP);
  pinMode(BUTTON_PREV, INPUT_PULLUP);
  pinMode(BUTTON_NEXT, INPUT_PULLUP);

  if (!musicPlayer.begin()) {
    if (DEBUG) Serial.println("VS1053 NOT FOUND");
    while (1);
  }
  if (DEBUG) Serial.println("VS1053 OK");

  if (!SD.begin(SDCS)) {
    if (DEBUG) Serial.println("SD FAIL");
    while (1);
  }
  if (DEBUG) Serial.println("SD OK");

  musicPlayer.setVolume(20, 20);

  scanSD();
  updateTrackFiles();
  playCurrentTrack();
}

void loop() {
  if (musicPlayer.playingMusic)
    musicPlayer.feedBuffer();

  // Auto next
  if (!musicPlayer.playingMusic && !isPaused) {
    currentTrack++;
    if (currentTrack >= totalTracks) currentTrack = 0;
    updateTrackFiles();
    playCurrentTrack();
  }

  // Volume
  int potValue = analogRead(POT_PIN);
  int volume = map(potValue, 0, 1023, 100, 0);
  musicPlayer.setVolume(volume, volume);

  // Button 
  if (checkButton(playBtn)) {
    if (isPaused) {
      musicPlayer.pausePlaying(false);
      if (DEBUG) Serial.println("RESUME");
      isPaused = false;
    } else {
      musicPlayer.pausePlaying(true);
      if (DEBUG) Serial.println("PAUSE");
      isPaused = true;
    }
  }

  if (checkButton(prevBtn)) {
    currentTrack--;
    if (currentTrack < 0) currentTrack = totalTracks - 1;
    updateTrackFiles();
    playCurrentTrack();
  }

  if (checkButton(nextBtn)) {
    currentTrack++;
    if (currentTrack >= totalTracks) currentTrack = 0;
    updateTrackFiles();
    playCurrentTrack();
  }
}
