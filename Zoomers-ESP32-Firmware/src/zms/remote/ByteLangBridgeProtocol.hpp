#pragma once

#include <Arduino.h>
#include <bytelang/bridge.hpp>


namespace zms {

struct ByteLangBridgeProtocol {
    using Sender = bytelang::bridge::Sender<rs::u8>;
    using Error = bytelang::bridge::Error;
    using Result = rs::Result<void, Error>;
    using Receiver = bytelang::bridge::Receiver<rs::u8, 1>;

    Sender sender;
    Receiver receiver;

    /// send_millis() -> u32
    bytelang::bridge::Instruction <Sender::Code> send_millis;

    /// send_log(...) -> u16[u8]
    bytelang::bridge::Instruction <Sender::Code, rs::str, rs::size> send_log;

    explicit ByteLangBridgeProtocol(Stream &arduino_stream) :
        sender{bytelang::core::OutputStream{arduino_stream}},
        receiver{
            .in = bytelang::core::InputStream{arduino_stream},
            .handlers = getInstructions(),
        },
        send_millis{
            sender.createInstruction(
                [](bytelang::core::OutputStream &stream) -> Result {
                    if (not stream.write(rs::u32(millis()))) {
                        return {Error::InstructionArgumentWriteFail};
                    }

                    return {};
                })
        },
        send_log{
            sender.createInstruction<rs::str, rs::size>(
                [](bytelang::core::OutputStream &stream, rs::str buffer, rs::size size) -> Result {
                    if (not stream.write(rs::u16(size))) {
                        return {Error::InstructionArgumentWriteFail};
                    }

                    if (not stream.write(buffer, size)) {
                        return {Error::InstructionArgumentWriteFail};
                    }

                    return {};
                })
        } {}

private:
    Receiver::InstructionTable getInstructions() {
        return {
            /// 0x00
            /// get_millis()
            [this](bytelang::core::InputStream &stream) -> Result {
                return send_millis();
            },

            //
        };
    }
};

}  // namespace zms