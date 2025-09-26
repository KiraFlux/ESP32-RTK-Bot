#include <Arduino.h>
#include <kf/Logger.hpp>

#include "zms/Robot.hpp"


void setup() {
    Serial.begin(115200);

    static auto &robot = zms::Robot::instance();

    kf::Logger::instance().write_func = [](const char *buffer, size_t size) {
        robot.bridge.send_log(buffer, size);
    };

    kf_Logger_info("begin");

    if (not robot.init()) {
        kf_Logger_fatal("Robot init failed!");
        robot.storage.erase();
        delay(5000);
        ESP.restart();
    }

    kf_Logger_debug("init ok");
}

void loop() {
    static auto &robot = zms::Robot::instance();
    robot.poll();

    delay(1);
}
