#include <Arduino.h>

#include "tools/Logger.hpp"

void setup() {

  Serial.begin(115200);

  Logger::instance().write_func = [](const char *text, size_t lenght) {
    Serial.write(text, lenght);
  };

  Logger_info("Start");
}

void loop() {
}
