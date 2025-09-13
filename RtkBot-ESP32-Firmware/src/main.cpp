#include <Arduino.h>

#include "hardware/Robot.hpp"

#include "kf/Logger.hpp"


void setup() {
    Serial.begin(115200);

    kf_Logger_info("begin");

    kf::Logger::instance().write_func = [](const char *text, size_t length) { Serial.write(text, length); };

    Robot::instance().init();
    Robot::Settings::storage().load();

    kf_Logger_debug("init ok");
}

void loop() { delay(5000); }
