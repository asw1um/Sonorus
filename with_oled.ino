#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>
#include <U8x8lib.h>
#include <Wire.h>

// ------------------ Pins ------------------
#define VS1053_RESET 9
#define VS1053_CS    10
#define VS1053_DCS   8
#define VS1053_DREQ  3
#define SDCS         4

#define BUTTON_PLAY  5
#define BUTTON_PREV  6
#define BUTTON_NEXT  7
#define POT_PIN      A0

#define DEBOUNCE_TIME 50
#define MAX_FILENAME 13

// ------------------ OLED ------------------
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(U8X8_PIN_NONE);

// ------------------ VS1053 ------------------
Adafruit_VS1053_FilePlayer musicPlayer(
  VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, SDCS);

// ------------------ Buttons ------------------
struct Button {
  int pin;
  bool lastReading;
  bool state;
  unsigned long lastDebounceTime;
};

Button playBtn = {BUTTON_PLAY, HIGH, HIGH, 0};
Button prevBtn = {BUTTON_PREV, HIGH, HIGH, 0};
Button nextBtn = {BUTTON_NEXT, HIGH, HIGH, 0};

bool isPaused = false;

// ------------------ Track System (Node) ------------------
int totalTracks = 0;
int currentTrack = 0;

char currentTrackFile[MAX_FILENAME] = {0};
char nextTrackFile[MAX_FILENAME] = {0};
char prevTrackFile[MAX_FILENAME] = {0};

// ------------------ Scroll System ------------------
unsigned long lastScrollTime = 0;
int scrollIndex = 0;
const int visibleChars = 8;   // full width
const int scrollDelay = 300;

// ------------------ SD Scan ------------------
void scanSD() {
  File root = SD.open("/");
  totalTracks = 0;

  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;

    if (!entry.isDirectory()) {
      String name = entry.name();
      name.toLowerCase();
      if (name.endsWith(".mp3"))
        totalTracks++;
    }
    entry.close();
  }
  root.close();

  if (totalTracks == 0) while (1);
}

// ------------------ Get Track ------------------
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

// ------------------ Update Node ------------------
void updateTrackFiles() {
  getTrackFilename(currentTrack, currentTrackFile);
  getTrackFilename((currentTrack + 1) % totalTracks, nextTrackFile);
  getTrackFilename((currentTrack - 1 + totalTracks) % totalTracks, prevTrackFile);
}

// ------------------ OLED UI (No Flash, Cut .mp3) ------------------
void updateOLED() {

  int len = strlen(currentTrackFile);

  // Cut off ".mp3" if present
  int nameLen = len;
  if (nameLen > 4) nameLen -= 4;

  char nameOnly[MAX_FILENAME];
  strncpy(nameOnly, currentTrackFile, nameLen);
  nameOnly[nameLen] = '\0';

  char line[visibleChars + 1];
  line[visibleChars] = '\0';

  for (int i = 0; i < visibleChars; i++) {
    if (nameLen <= visibleChars)
      line[i] = (i < nameLen) ? nameOnly[i] : ' ';
    else
      line[i] = nameOnly[(scrollIndex + i) % nameLen];
  }

  // ---- Top line ----
  u8x8.setCursor(0, 0);
  u8x8.print(line);

  // ---- Status line ----
  u8x8.setCursor(0, 2);
  if (isPaused)
    u8x8.print("PAUSE || ");
  else
    u8x8.print("PLAY  > ");
}

// ------------------ Play Track ------------------
void playCurrentTrack() {
  musicPlayer.stopPlaying();
  musicPlayer.startPlayingFile(currentTrackFile);
  isPaused = false;
  scrollIndex = 0;
  updateOLED();
}

// ------------------ Button Debounce ------------------
bool checkButton(Button &btn) {
  bool reading = digitalRead(btn.pin);

  if (reading != btn.lastReading)
    btn.lastDebounceTime = millis();

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

// ------------------ Setup ------------------
void setup() {
  pinMode(10, OUTPUT);
  pinMode(BUTTON_PLAY, INPUT_PULLUP);
  pinMode(BUTTON_PREV, INPUT_PULLUP);
  pinMode(BUTTON_NEXT, INPUT_PULLUP);

  if (!musicPlayer.begin()) while (1);
  if (!SD.begin(SDCS)) while (1);

  musicPlayer.setVolume(20, 20);

  scanSD();
  updateTrackFiles();
  playCurrentTrack();

  Wire.begin();
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_8x13B_1x2_f);

  updateOLED();
}

// ------------------ Loop ------------------
void loop() {

  if (musicPlayer.playingMusic)
    musicPlayer.feedBuffer();

  // Auto next track
  if (!musicPlayer.playingMusic && !isPaused) {
    currentTrack = (currentTrack + 1) % totalTracks;
    updateTrackFiles();
    playCurrentTrack();
  }

  // Volume
  int potValue = analogRead(POT_PIN);
  int volume = map(potValue, 0, 1023, 100, 0);
  musicPlayer.setVolume(volume, volume);

  // Scroll timing
  if (millis() - lastScrollTime > scrollDelay) {
    lastScrollTime = millis();
    scrollIndex++;
    updateOLED();
  }

  // Buttons
  if (checkButton(playBtn)) {
    isPaused = !isPaused;
    musicPlayer.pausePlaying(isPaused);
    updateOLED();
  }

  if (checkButton(prevBtn)) {
    currentTrack = (currentTrack - 1 + totalTracks) % totalTracks;
    updateTrackFiles();
    playCurrentTrack();
  }

  if (checkButton(nextBtn)) {
    currentTrack = (currentTrack + 1) % totalTracks;
    updateTrackFiles();
    playCurrentTrack();
  }
}
