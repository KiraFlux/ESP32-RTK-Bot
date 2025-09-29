#include <Arduino.h>
#include <kf/Logger.hpp>

#include "zms/Periphery.hpp"
#include "zms/Service.hpp"

void setup() {
    Serial.begin(115200);

    kf::Logger::instance().write_func = [](const char *buffer, size_t size) {
        static auto &bridge = zms::Service::instance().bytelang_bridge;
        bridge.send_log(buffer, size);
    };

    kf_Logger_info("begin");

    auto &periphery = zms::Periphery::instance();
    auto &service = zms::Service::instance();

    const bool periphery_ok = periphery.init();
    const bool service_ok = service.init();

    if (not periphery_ok or not service_ok) {
        kf_Logger_fatal("Robot init failed!");
        periphery.storage.erase();
        delay(5000);
        ESP.restart();
    }

    kf_Logger_debug("init ok");
}

void loop() {}
