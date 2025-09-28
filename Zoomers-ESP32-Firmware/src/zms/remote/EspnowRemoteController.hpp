#pragma once

#include "zms/tools/TimeoutManager.hpp"

namespace zms {

/// Работает с пакетами данных с пульта
struct EspnowRemoteController {

    /// Пакет данных управления
    struct ControlPacket {
        float left_x{0}, left_y{0};
        float right_x{0}, right_y{0};
        bool toggle_mode{false};
    };

    ControlPacket packet{};

private:
    TimeoutManager packet_timeout_manager{200};

public:
    bool disconnected{true};

    /// Рассчитать управление для двух моторов
    void calc(float &ret_left, float &ret_right) const {
    }

    /// Просрочен ли пакет
    [[nodiscard]] inline bool isPacketTimeoutExpired() const { return packet_timeout_manager.expired(); }

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
