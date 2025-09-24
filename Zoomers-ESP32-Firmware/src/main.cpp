#include <Arduino.h>
#include <kf/Logger.hpp>

#include <bytelang/bridge.hpp>

#include "zms/Robot.hpp"

namespace zms {

/// Реализация протокола моста ByteLang для ZMS
struct ByteLangBridgeProtocol : Singleton<ByteLangBridgeProtocol> {
    friend struct Singleton<ByteLangBridgeProtocol>;

    using Receiver = bytelang::bridge::Receiver<rs::u8, 1>;
    using Sender = bytelang::bridge::Sender<rs::u8>;
    using Instruction = bytelang::bridge::Instruction<Sender::Code>;
    using InstructionResult = rs::Result<void, bytelang::bridge::Error>;

    Receiver receiver{
        .in = bytelang::core::InputStream{Serial},
        .handlers = {{
            /// 0x00: get_millis()
            [this](bytelang::core::InputStream &stream) -> InstructionResult {
                return send_millis();
            }

            //
        }}};

    Sender sender{bytelang::core::OutputStream{Serial}};

    /// 0x00 : Отправить время в мс
    Instruction send_millis{sender.createInstruction([](bytelang::core::OutputStream &stream) -> InstructionResult {
        const auto value = rs::u32(millis());
        if (not stream.write(value)) { return {bytelang::bridge::Error::InstructionArgumentWriteFail}; }

        return {};
    })};
};

}// namespace zms

void setup() {
    Serial.begin(115200);
    kf::Logger::instance().write_func = [](const char *buffer, size_t size) { Serial.write(buffer, size); };
    kf_Logger_info("begin");

    zms::Robot::instance().init();

    kf_Logger_debug("init ok");
}

void loop() {
    static auto &robot = zms::Robot::instance();
    robot.poll();

    static auto &bridge = zms::ByteLangBridgeProtocol::instance();
    const auto result = bridge.receiver.poll();
    if (result.fail()) { kf_Logger_error("BL bridge error: %d", static_cast<int>(result.error)); }

    delay(1);
}
