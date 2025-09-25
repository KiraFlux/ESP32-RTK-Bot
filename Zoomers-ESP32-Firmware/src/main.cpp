#include <Arduino.h>
#include <kf/Logger.hpp>

#include "zms/Robot.hpp"

void setup() {
    static auto &robot = zms::Robot::instance();

    Serial.begin(115200);

    kf::Logger::instance().write_func = [](const char *buffer, size_t size) { 
        // robot.bridge.send_log(buffer, size);        
    };

    kf_Logger_info("begin");

    robot.init();

    kf_Logger_debug("init ok");
}

void loop() {
    static auto &robot = zms::Robot::instance();
    robot.poll();

    delay(1);
}
