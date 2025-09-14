#pragma once

#include "tools/Singleton.hpp"

#include "hardware/Motor.hpp"
#include "hardware/Espnow.hpp"
#include "kf/Storage.hpp"


/// @brief Аппаратное обеспечение робота
struct Robot : Singleton<Robot> {
    friend struct Singleton<Robot>;

    /// Настройки аппаратного обеспечения
    struct Settings {
        /// Настройки моторов
        Motor::Settings left_motor, right_motor;
        /// Настройки узла Espnow
        Espnow::Settings esp_now;
    };

    /// Хранилище настроек
    kf::Storage<Settings> storage{
        .key="RobotSet",
        .settings={
            .left_motor={
                .max_pwm=255,
                .direction=Motor::Settings::Direction::CCW
            },
            .right_motor={
                .max_pwm=255,
                .direction=Motor::Settings::Direction::CW
            },
            .esp_now={
                .remote_controller_mac={0x78, 0x1c, 0x3c, 0xa4, 0x96, 0xdc}
            }
        }
    };

    /// Левый мотор
    Motor left_motor{storage.settings.left_motor, GPIO_NUM_27, GPIO_NUM_21};

    /// Правый мотор
    Motor right_motor{storage.settings.right_motor, GPIO_NUM_19, GPIO_NUM_18};

    /// Узел протокола Espnow
    Espnow esp_now{storage.settings.esp_now};

    /// Инициализировать всю периферию
    bool init() {
        left_motor.init();
        right_motor.init();

        if (not storage.load() and not storage.save()) { return false; }

        if (not esp_now.init()) { return false; }

        return true;
    }
};
