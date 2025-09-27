#include <Arduino.h>
#include <kf/Logger.hpp>

#include "zms/Robot.hpp"
#include "zms/remote/ByteLangBridgeProtocol.hpp"

void setup() {
    Serial.begin(115200);

    static auto &robot = zms::Robot::instance();
    static auto &bridge = zms::ByteLangBridgeProtocol::instance();

    kf::Logger::instance().write_func = [](const char *buffer, size_t size) {
        bridge.send_log(buffer, size);
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
    delay(1);

    static auto &robot = zms::Robot::instance();
    robot.poll();

    static auto &bridge = zms::ByteLangBridgeProtocol::instance();
    const auto result = bridge.receiver.poll();
    if (result.fail()) {
        kf_Logger_error("BL bridge error: %d", static_cast<int>(result.error));
    }
}
