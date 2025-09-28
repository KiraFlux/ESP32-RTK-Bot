#include <Arduino.h>
#include <kf/Logger.hpp>

#include "zms/Robot.hpp"


// espnow

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

    auto translate = [](Action code) {
        using kf::tui::Event;

        switch (code) {
            case Action::Reload: return Event::Update;
            case Action::Click: return Event::Click;
            case Action::Left: return Event::ChangeIncrement;
            case Action::Right: return Event::ChangeDecrement;
            case Action::Up: return Event::ElementPrevious;
            case Action::Down: return Event::ElementNext;
            case Action::None:
            default: return Event::None;
        }
    };

    static auto &page_manager = kf::tui::PageManager::instance();

    switch (size) {
        case sizeof(zms::EspnowRemoteController::ControlPacket)://
            espnow_remote_controller.updateControlPacket(*static_cast<const zms::EspnowRemoteController::ControlPacket *>(data));
            return;

        case sizeof(Action)://
            page_manager.addEvent(translate(*static_cast<const Action *>(data)));
            return;

        default: kf_Logger_warn("Unknown packet: (%d bytes)", size);
    }
}

void setup() {
    Serial.begin(115200);

    kf::Logger::instance().write_func = [](const char *buffer, size_t size) {
        static auto &bridge = zms::ByteLangBridgeProtocol::instance();
        bridge.send_log(buffer, size);
    };

    kf_Logger_info("begin");

    auto &periphery = zms::Periphery::instance();
    if (not periphery.init()) {
        kf_Logger_fatal("Robot init failed!");
        periphery.storage.erase();
        delay(5000);
        ESP.restart();
    }

    periphery.espnow_node.on_receive = onEspnowRemoteControllerPacket;

    kf_Logger_debug("init ok");
}

void loop() {}
