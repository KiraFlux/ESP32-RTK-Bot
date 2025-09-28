#pragma once

#include <Arduino.h>
#include <bytelang/bridge.hpp>

#include "zms/Robot.hpp"
#include "zms/tools/Singleton.hpp"

namespace zms {

struct ByteLangBridgeProtocol : Singleton<ByteLangBridgeProtocol> {
    friend struct Singleton<ByteLangBridgeProtocol>;

    using Sender = bytelang::bridge::Sender<rs::u8>;
    using Error = bytelang::bridge::Error;
    using Result = rs::Result<void, Error>;
    using Receiver = bytelang::bridge::Receiver<rs::u8, 2>;

    Sender sender;
    Receiver receiver;

    /// send_millis() -> u32
    bytelang::bridge::Instruction<Sender::Code> send_millis;

    /// send_log(...) -> u16[u8]
    bytelang::bridge::Instruction<Sender::Code, rs::str, rs::size> send_log;

    /// task_completed() -> u32
    bytelang::bridge::Instruction<Sender::Code, rs::u32> task_completed;

    explicit ByteLangBridgeProtocol() :
        sender{bytelang::core::OutputStream{Serial}},
        receiver{
            .in = bytelang::core::InputStream{Serial},
            .handlers = getInstructions(),
        },
        send_millis{
            sender.createInstruction(
                [](bytelang::core::OutputStream &stream) -> Result {
                    if (not stream.write(rs::u32(millis()))) {
                        return {Error::InstructionArgumentWriteFail};
                    }

                    return {};
                })},
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
                })},
        task_completed{
            sender.createInstruction<rs::u32>([](bytelang::core::OutputStream &stream, rs::u32 result) -> Result {
                if (not stream.write(result)) { return Error::InstructionArgumentWriteFail; }

                kf_Logger_debug("task completed with result=%d", result);

                return {};
            })} {}

    [[nodiscard]] bool init() {
        const auto result = xTaskCreate(
            ByteLangBridgeProtocol::taskHandler,
            "bl-bridge",
            4096,
            this,
            1,
            nullptr);

        return result == pdPASS;
    }

private:
    [[noreturn]] static void taskHandler(void *instance) {
        auto &bridge = *static_cast<ByteLangBridgeProtocol *>(instance);

        while (true) {
            const auto result = bridge.receiver.poll();
            if (result.fail()) {
                kf_Logger_error("BL bridge error: %d", static_cast<int>(result.error));
            }
            delay(1);
        }
    }

    Receiver::InstructionTable getInstructions() {
        return {
            /// 0x00
            /// get_millis()
            [this](bytelang::core::InputStream &stream) -> Result {
                return send_millis();
            },

            /// 0x01
            /// execute_task(u8 code, f32 arg)
            [this](bytelang::core::InputStream &stream) -> Result {
                auto &robot = Robot::instance();

                const auto code_opt = stream.readByte();
                if (code_opt.none()) { return Error::InstructionArgumentReadFail; }

                const rs::u8 task_code = code_opt.value;
                const rs::f32 arg = stream.read<rs::f32>().value;
                
                kf_Logger_info("Executing task=%d, arg=%f", task_code, arg);

                robot.task.arg = arg;
                robot.executeTask(static_cast<Task::State>(task_code));

                while (robot.isExecutingTask()) {
                    delay(1);
                }

                return task_completed(robot.task.result);
            },

            //
        };
    }
};

}// namespace zms