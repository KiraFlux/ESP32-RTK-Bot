#include <Arduino.h>
#include <kf/Logger.hpp>
#include <kf/tui.hpp>

#include "zms/Robot.hpp"
#include "zms/remote/RemoteController.hpp"
#include "zms/tools/Timer.hpp"
#include "zms/ui/pages/EncoderTunePage.hpp"
#include "zms/ui/pages/MainPage.hpp"
#include "zms/ui/pages/MotorTunePage.hpp"
#include "zms/ui/pages/RobotSettingsPage.hpp"

static void onEspnowRemoteControllerPacket(const void *data, rs::u8 size) {

    /// Действие в меню
    enum Action : rs::u8 {
        None = 0x00,
        Reload = 0x10,
        Click = 0x20,
        Left = 0x30,
        Right = 0x31,
        Up = 0x40,
        Down = 0x41
    };

    auto translate = [](Action code) -> kf::Event {
        switch (code) {
            case Action::Reload: return kf::Event::Update;
            case Action::Click: return kf::Event::Click;
            case Action::Left: return kf::Event::ChangeIncrement;
            case Action::Right: return kf::Event::ChangeDecrement;
            case Action::Up: return kf::Event::ElementPrevious;
            case Action::Down: return kf::Event::ElementNext;
            case Action::None:
            default: return kf::Event::None;
        }
    };

    switch (size) {
        case sizeof(zms::RemoteController::ControlPacket)://
            zms::RemoteController::instance().updateControlPacket(*static_cast<const zms::RemoteController::ControlPacket *>(data));
            return;

        case sizeof(Action)://
            kf::PageManager::instance().addEvent(translate(*static_cast<const Action *>(data)));
            return;

        default: kf_Logger_warn("Unknown packet: (%d bytes)", size);
    }
}

static void sendTUI(kf::PageManager &page_manager) {
    const auto slice = page_manager.render();

    const auto result = zms::Robot::instance().espnow_node.send(
        slice.data,
        std::min(slice.len, static_cast<rs::size>(ESP_NOW_MAX_DATA_LEN)));

    kf_Logger_error("Send: %d bytes", slice.len);

    if (result.fail()) { kf_Logger_error(rs::toString(result.error)); }
}

static void setupTUI(kf::PageManager &page_manager) {
    auto &robot = zms::Robot::instance();
    static zms::RobotSettingsPage robot_settings_page{robot.storage};
    static zms::MotorTunePage left_motor_tune_page{"Tune-MotorLeft", robot.left_motor, robot.storage};
    static zms::MotorTunePage right_motor_tune_page{"Tune-MotorRight", robot.right_motor, robot.storage};
    static zms::EncoderTunePage left_encoder_tune_page{"Tune-EncoderLeft", robot.left_encoder};
    static zms::EncoderTunePage right_encoder_tune_page{"Tune-EncoderRight", robot.right_encoder};

    page_manager.bind(zms::MainPage::instance());
}

static void pollRemoteControl() {
    static auto &remote_controller = zms::RemoteController::instance();
    static auto &robot = zms::Robot::instance();
    static bool disconnected{true};

    if (remote_controller.isPacketTimeoutExpired()) {
        if (not disconnected) {
            kf_Logger_info("disconnected");
            disconnected = true;

            remote_controller.resetControlPacket();
            robot.left_motor.stop();
            robot.right_motor.stop();
        }
    } else {
        disconnected = false;

        float left_power, right_power;
        remote_controller.calc(left_power, right_power);
        robot.left_motor.set(left_power);
        robot.right_motor.set(right_power);
    }
}

void setup() {
    Serial.begin(115200);
    kf::Logger::instance().write_func = [](const char *buffer, size_t size) { Serial.write(buffer, size); };
    kf_Logger_info("begin");

    auto &robot = zms::Robot::instance();
    robot.init();
    robot.espnow_node.on_receive = onEspnowRemoteControllerPacket;

    auto &page_manager = kf::PageManager::instance();
    setupTUI(page_manager);
    sendTUI(page_manager);

    kf_Logger_debug("init ok");
}

void loop() {
    static auto &page_manager = kf::PageManager::instance();
    static auto &robot = zms::Robot::instance();

    static zms::Timer timer{100};

    if (timer.ready()) {
        kf_Logger_debug("L: %d\tR: %d", robot.left_encoder.position, robot.right_encoder.position);
    }

    const bool update_required = page_manager.pollEvents();
    if (update_required) { sendTUI(page_manager); }

    pollRemoteControl();

    delay(1);
}
