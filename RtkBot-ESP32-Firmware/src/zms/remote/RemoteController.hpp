#pragma once

#include "zms/tools/Singleton.hpp"
#include "zms/tools/TimeoutManager.hpp"

/// Работает с пакетами данных с пульта
struct RemoteController : Singleton<RemoteController> {
    friend struct Singleton<RemoteController>;

    /// Пакет данных управления
    struct ControlPacket {
        float left_x{0}, left_y{0};
        float right_x{0}, right_y{0};
        bool toggle_mode{false};
    };

private:
    ControlPacket control_packet{};
    TimeoutManager packet_timeout_manager{1000};

public:
    /// Рассчитать управление для двух моторов
    void calc(float &ret_left, float &ret_right) const {
        if (control_packet.toggle_mode) {
            // Tank mode
            ret_left = control_packet.left_y;
            ret_right = control_packet.right_y;
        } else {
            // System mode
            ret_left = control_packet.left_y + control_packet.left_x;
            ret_right = control_packet.left_y - control_packet.left_x;
        }
    }

    /// Просрочен ли пакет
    inline bool isPacketTimeoutExpired() const { return packet_timeout_manager.expired(); }

    /// Сбросить значение пакета управления
    void resetControlPacket() {
        control_packet = ControlPacket{};
    }

    /// Обновить пакет
    void updateControlPacket(const ControlPacket &packet) {
        control_packet = packet;
        packet_timeout_manager.update();
    }
};