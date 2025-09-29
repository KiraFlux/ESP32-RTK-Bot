#pragma once

#include "zms/Periphery.hpp"
#include "zms/services/ByteLangBridgeProtocol.hpp"
#include "zms/services/DualJoystickRemoteController.hpp"
#include "zms/services/TextUI.hpp"
#include "zms/tools/Singleton.hpp"

namespace zms {

/// @brief Единственная и глобальная точках входа и связывания всех дополнительных сервисов робота
struct Service final : Singleton<Service> {
    friend struct Singleton<Service>;

    /// @brief Менеджер текстового пользоввательского интерфейса
    TextUI text_ui{};

    /// @brief Удаленный контроллер
    DualJoystickRemoteController dual_joystick_remote_controller{200};

    /// @brief ByteLang мост
    ByteLangBridgeProtocol bytelang_bridge{};

    /// @brief Инициаоизация сервисов
    [[nodiscard]] bool init() {
        auto &periphery = zms::Periphery::instance();

        periphery.espnow_node.on_receive = [this](const void *data, rs::u8 size) {
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

            auto translateActionToEvent = [](Action code) {
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

            switch (size) {
                case sizeof(zms::DualJoystickRemoteController::ControlPacket)://
                    dual_joystick_remote_controller.updateControlPacket(*static_cast<const zms::DualJoystickRemoteController::ControlPacket *>(data));
                    return;

                case sizeof(Action)://
                    text_ui.addEvent(translateActionToEvent(*static_cast<const Action *>(data)));
                    return;

                default: kf_Logger_warn("Unknown packet: (%d bytes)", size);
            }
        };

        dual_joystick_remote_controller.handler = [](const DualJoystickRemoteController::ControlPacket &packet) {
            kf_Logger_debug(
                "Packet:"
                "L(%f\t%f)\t"
                "R(%f\t%f)\t"
                "mode %s",
                packet.left_x,
                packet.left_y,
                packet.right_x,
                packet.right_y,
                packet.mode);
        };

        return true;
    }

    /// @brief Прокрутка событий сервисов
    void poll() {
        text_ui.poll();
        bytelang_bridge.poll();
        dual_joystick_remote_controller.poll();
    };
};

}// namespace zms
