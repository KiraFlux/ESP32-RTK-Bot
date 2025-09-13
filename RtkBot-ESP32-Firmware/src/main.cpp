#include <Arduino.h>

#include "hardware/Robot.hpp"

#include "kf/Logger.hpp"


void setup() {
    Serial.begin(115200);

    kf_Logger_info("begin");

    kf::Logger::instance().write_func = [](const char *buffer, size_t size) { Serial.write(buffer, size); };

    auto &robot = Robot::instance();
    robot.init();


    auto &robot_storage = Robot::Settings::storage();
    if (not robot_storage.load()) {
        robot_storage.save();
    }

    robot.esp_now.on_receive = [](const void *, rs::u8 size) {
        kf_Logger_info("got %d bytes", size);
    };

    kf_Logger_debug("init ok");
}

void loop() {
    kf_Logger_debug("beep: %ld", millis());
    delay(5000);
}
