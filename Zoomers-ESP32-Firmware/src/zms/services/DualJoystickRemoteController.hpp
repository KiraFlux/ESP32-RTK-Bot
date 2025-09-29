#pragma once

#include <functional>
#include <kf/Logger.hpp>

#include "zms/tools/TimeoutManager.hpp"

namespace zms {

/// Работает с пакетами данных с пульта
struct DualJoystickRemoteController {

    /// Пакет данных управления
    struct ControlPacket {
        float left_x{0}, left_y{0};
        float right_x{0}, right_y{0};
        bool toggle_mode{false};
    };

private:
    /// @brief Менеджер тайм-аута пакета
    TimeoutManager packet_timeout_manager;

    /// @brief Флаг отключения
    bool disconnected{true};

    /// @brief Актуальный пакет данных
    ControlPacket packet{};

public:
    /// @brief Обработчик входящего пакета
    std::function<void(const ControlPacket &)> handler{nullptr};

    explicit DualJoystickRemoteController(Milliseconds packet_timeout) :
        packet_timeout_manager{packet_timeout} {}

    /// @brief Прокрутка событий
    void poll() {
        if (packet_timeout_manager.expired()) {
            if (not disconnected) {
                kf_Logger_info("disconnected");

                disconnected = true;
                resetControlPacket();
            }
        } else {
            if (handler) {
                handler(packet);
            }
        }
    }

    /// Сбросить значение пакета управления
    void resetControlPacket() {
        packet = ControlPacket{};
    }

    /// Обновить пакет
    void updateControlPacket(const ControlPacket &p) {
        packet = p;
        packet_timeout_manager.update();
    }
};

}// namespace zms
