#pragma once

#include <Arduino.h>
#include <bytelang/bridge.hpp>

namespace zms {

/// Реализация протокола моста ByteLang для ZMS
struct ByteLangBridgeProtocol {

    using Sender = bytelang::bridge::Sender<rs::u8>;
    using Receiver = bytelang::bridge::Receiver<rs::u8, 1>;
    using Instruction = bytelang::bridge::Instruction<Sender::Code>;
    using InstructionResult = rs::Result<void, bytelang::bridge::Error>;

    Sender sender;

    Receiver receiver;

    ByteLangBridgeProtocol(Stream &arduino_stream) :
        sender{bytelang::core::OutputStream{arduino_stream}},
        receiver{
            .in = bytelang::core::InputStream{arduino_stream},
            .handlers = getInstructions(),
        } {}

    /// 0x00
    /// Отправить время в мс
    Instruction send_millis{sender.createInstruction([](bytelang::core::OutputStream &stream) -> InstructionResult {
        const auto value = rs::u32(millis());
        if (not stream.write(value)) { return {bytelang::bridge::Error::InstructionArgumentWriteFail}; }

        return {};
    })};

    /// 0x01
    /// Отправить лог
    Instruction send_log{sender.createInstruction([](bytelang::core::OutputStream &stream) -> InstructionResult {
        return {};
    })};

private:
    Receiver::InstructionTable getInstructions() {
        return {
            /// 0x00
            /// get_millis()
            [this](bytelang::core::InputStream &stream) -> InstructionResult {
                return send_millis();
            },

            //
        };
    }
};

}// namespace zms