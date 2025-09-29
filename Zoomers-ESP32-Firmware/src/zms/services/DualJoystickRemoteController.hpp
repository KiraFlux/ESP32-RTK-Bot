#pragma once

#include <functional>
#include <kf/Logger.hpp>

#include "zms/aliases.hpp"
#include "zms/tools/TimeoutManager.hpp"

namespace zms {

/// @brief Работает с пакетами данных с пульта
struct DualJoystickRemoteController {

    /// @brief Пакет данных управления
    struct ControlPacket {
        /// @brief Левый стик, ось X
        float left_x{0};

        /// @brief Левый стик, ось Y
        float left_y{0};

        /// @brief Правый стик, ось X
        float right_x{0};

        /// @brief Правый стик, ось Y
        float right_y{0};

        /// @brief Режим
        bool mode{false};
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

    /// @brief Сбросить значение пакета управления
    void resetControlPacket() {
        packet = ControlPacket{};
    }

    /// @brief Обновить пакет
    void updateControlPacket(const ControlPacket &p) {
        packet = p;
        packet_timeout_manager.update();
    }
};

}// namespace zms
