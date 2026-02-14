#include <SD.h>
#define SDCS 4

void setup() {
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  
  if (!SD.begin(SDCS)) {
    Serial.println("SD FAIL");
    while (1);
  }
  Serial.println("SD OK");

  File root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;
    Serial.println(entry.name());  // Should print correct short filenames
    entry.close();
  }
  root.close();
}

void loop() {}
