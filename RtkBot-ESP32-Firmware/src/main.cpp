#include <Arduino.h>

#include "hardware/Robot.hpp"

#include "kf/Text-UI.hpp"

#include "Alpha-UI.hpp"

#include "kf/Logger.hpp"


static void onEspnowRemoveControllerPacket(const void *data, rs::u8 size) {
    enum class Code : uint8_t {
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
            case Code::Reload:
                return kf::Event::Update;
            case Code::Click:
                return kf::Event::Click;
            case Code::Left:
                return kf::Event::ChangeIncrement;
            case Code::Right:
                return kf::Event::ChangeDecrement;
            case Code::Up:
                return kf::Event::ElementPrevious;
            case Code::Down:
                return kf::Event::ElementNext;
            case Code::None:
            default:
                return kf::Event::None;
        }
    };

    switch (size) {
        case 20: // DJC control packet
            return;

        case sizeof(Code):
            kf::PageManager::instance().addEvent(translate(*static_cast<const Code *>(data)));
            return;

        default:
            kf_Logger_warn("Unknown packet: (%d bytes)", size);
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
        static RobotSettingsPage robot_settings_page{Robot::Settings::storage()};
        static TestMotorPage test_left_motor_page{"Test-MotorLeft", robot.left_motor};
        static TestMotorPage test_right_motor_page{"Test-MotorRight", robot.right_motor};

        kf::PageManager::instance().bind(MainPage::instance());
    }

    auto &robot_storage = Robot::Settings::storage();
    if (not robot_storage.load()) {
        robot_storage.save();
    }

    kf_Logger_debug("init ok");
}

void loop() {
    static auto &page_manager = kf::PageManager::instance();

    if (page_manager.pollEvents()) {
        const auto slice = page_manager.render();
        const auto result = Robot::instance().esp_now.send(slice.data, slice.len);
        if (result.fail()) { kf_Logger_error(rs::toString(result.error)); }
    }
}
