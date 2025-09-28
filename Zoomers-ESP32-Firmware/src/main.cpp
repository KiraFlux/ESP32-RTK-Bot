#include <Arduino.h>
#include <kf/Logger.hpp>

#include "zms/Robot.hpp"

#include "zms/remote/ByteLangBridgeProtocol.hpp"

#include "zms/tools/Timer.hpp"

void setup() {
    Serial.begin(115200);

    static auto &robot = zms::Robot::instance();
    static auto &bridge = zms::ByteLangBridgeProtocol::instance();

    kf::Logger::instance().write_func = [](const char *buffer, size_t size) {
        bridge.send_log(buffer, size);
    };

    kf_Logger_info("begin");

    const auto robot_ok = robot.init();
    const auto bridge_ok = bridge.init();

    if (not robot_ok or not bridge_ok) {
        kf_Logger_fatal("Robot init failed!");
        robot.storage.erase();
        delay(5000);
        ESP.restart();
    }

    kf_Logger_debug("init ok");
}

void loop() {
    delay(1);

    static auto &robot = zms::Robot::instance();
    robot.poll();

    static zms::Timer log_timer{200};
}
