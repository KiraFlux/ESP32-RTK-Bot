#include <Arduino.h>
#include <kf/Logger.hpp>

#include "Alpha-UI.hpp"
#include "RemoteController.hpp"
#include "hardware/Robot.hpp"

static void onEspnowRemoveControllerPacket(const void *data, rs::u8 size) {
    enum class Code : rs::u8 {
        None = 0x00,
        Reload = 0x10,
        Click = 0x20,
        Left = 0x30,
        Right = 0x31,
        Up = 0x40,
        Down = 0x41
    };

    auto translate = [](Code code) -> kf::Event {
        switch (code) {
            case Code::Reload: return kf::Event::Update;
            case Code::Click: return kf::Event::Click;
            case Code::Left: return kf::Event::ChangeIncrement;
            case Code::Right: return kf::Event::ChangeDecrement;
            case Code::Up: return kf::Event::ElementPrevious;
            case Code::Down: return kf::Event::ElementNext;
            case Code::None:
            default: return kf::Event::None;
        }
    };

    switch (size) {
        case sizeof(RemoteController::ControlPacket)://
            RemoteController::instance().updateControlPacket(*static_cast<const RemoteController::ControlPacket *>(data));
            return;

        case sizeof(Code)://
            kf::PageManager::instance().addEvent(translate(*static_cast<const Code *>(data)));
            return;

        default: kf_Logger_warn("Unknown packet: (%d bytes)", size);
    }
}

void setup() {
    Serial.begin(115200);
    kf::Logger::instance().write_func = [](const char *buffer, size_t size) { Serial.write(buffer, size); };
    kf_Logger_info("begin");

    auto &robot = Robot::instance();
    robot.init();
    robot.esp_now.on_receive = onEspnowRemoveControllerPacket;

    // TUI setup
    {
        auto &storage = Robot::instance().storage;
        static RobotSettingsPage robot_settings_page{storage};
        static MotorTunePage test_left_motor_page{"Tune-MotorLeft", robot.left_motor, storage};
        static MotorTunePage test_right_motor_page{"Tune-MotorRight", robot.right_motor, storage};

        kf::PageManager::instance().bind(MainPage::instance());
    }

    kf_Logger_debug("init ok");
}

void loop() {
    static auto &page_manager = kf::PageManager::instance();
    static auto &dual_joystick = RemoteController::instance();
    static auto &robot = Robot::instance();

    if (page_manager.pollEvents()) {
        const auto slice = page_manager.render();
        const auto result = Robot::instance().esp_now.send(slice.data, slice.len);
        if (result.fail()) { kf_Logger_error(rs::toString(result.error)); }
    }

    // DJC
    {
        static bool disconnected{true};
        if (dual_joystick.isPacketTimeoutExpired()) {
            if (not disconnected) {
                kf_Logger_info("disconnected");
                disconnected = true;

                dual_joystick.resetControlPacket();
                robot.left_motor.stop();
                robot.right_motor.stop();
            }
        } else {
            disconnected = false;

            float left_power, right_power;
            dual_joystick.calc(left_power, right_power);
            robot.left_motor.set(left_power);
            robot.right_motor.set(right_power);
        }
    }

    delay(1);
}
