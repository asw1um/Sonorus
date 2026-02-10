#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================= DFPLAYER =================
SoftwareSerial mp3Serial(10, 11); // RX, TX
DFRobotDFPlayerMini mp3;

// ================= PINS =================
#define BTN_PLAY 2
#define BTN_NEXT 3
#define BTN_PREV 4
#define POT_VOL  A0

// ================= TRACK DATA =================
// Index 0 = Track 1 (0001.mp3)
const char* trackNames[] = {
  "Boot Sound",
  "Lo-fi Beat",
  "Night Drive"
};

const int TOTAL_TRACKS = sizeof(trackNames) / sizeof(trackNames[0]);

// ================= STATE =================
int currentTrack = 1;
bool isPlaying = true;
int volume = 20;

// ================= TIMING =================
unsigned long lastButtonTime = 0;
const unsigned long debounceDelay = 200;

void setup() {
  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);

  Serial.begin(9600);
  mp3Serial.begin(9600);

  // OLED init
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // DFPlayer init
  if (!mp3.begin(mp3Serial)) {
    showError("DFPlayer ERROR");
    while (1);
  }

  mp3.volume(volume);
  mp3.play(currentTrack);

  updateDisplay();
}

void loop() {
  readButtons();
  readVolume();
}

// ================= INPUT =================
void readButtons() {
  if (millis() - lastButtonTime < debounceDelay) return;

  if (!digitalRead(BTN_PLAY)) {
    lastButtonTime = millis();
    togglePlay();
  }

  if (!digitalRead(BTN_NEXT)) {
    lastButtonTime = millis();
    nextTrack();
  }

  if (!digitalRead(BTN_PREV)) {
    lastButtonTime = millis();
    prevTrack();
  }
}

void readVolume() {
  int potValue = analogRead(POT_VOL);
  int newVolume = map(potValue, 0, 1023, 0, 30);

  if (newVolume != volume) {
    volume = newVolume;
    mp3.volume(volume);
    updateDisplay();
  }
}

// ================= PLAYER CONTROL =================
void togglePlay() {
  if (isPlaying) {
    mp3.pause();
    isPlaying = false;
  } else {
    mp3.start();
    isPlaying = true;
  }
  updateDisplay();
}

void nextTrack() {
  currentTrack++;
  if (currentTrack > TOTAL_TRACKS) currentTrack = 1;

  mp3.play(currentTrack);
  isPlaying = true;
  updateDisplay();
}

void prevTrack() {
  currentTrack--;
  if (currentTrack < 1) currentTrack = TOTAL_TRACKS;

  mp3.play(currentTrack);
  isPlaying = true;
  updateDisplay();
}

// ================= DISPLAY =================
void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);

  display.print("Track ");
  display.print(currentTrack);
  display.print("/");
  display.println(TOTAL_TRACKS);

  display.println(trackNames[currentTrack - 1]);

  display.print("Volume: ");
  display.println(volume);

  display.print("Status: ");
  display.println(isPlaying ? "PLAY" : "PAUSE");

  display.display();
}

void showError(const char* msg) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(msg);
  display.display();
}
