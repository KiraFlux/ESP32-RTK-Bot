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

        /// Параметры левого мотора
        Motor::Settings left{
            .max_pwm=255,
            .direction=Motor::Settings::Direction::CCW
        };

        /// Параметры правого мотора
        Motor::Settings right{
            .max_pwm=255,
            .direction=Motor::Settings::Direction::CW
        };

        /// Параметры протокола Espnow
        Espnow::Settings esp_now{
            .remote_controller_mac={0x78, 0x1c, 0x3c, 0xa4, 0x96, 0xdc}
        };

        /// Получить ссылку на Singleton экземпляр хранилища настроек
        static kf::Storage<Settings> &storage() {
            static kf::Storage<Settings> instance{
                .key="RobotSet",
                .settings = {}
            };
            return instance;
        }

        Settings(const Settings &) = delete;

    private:
        Settings() = default;
    };

    /// Левый мотор
    Motor left_motor{Settings::storage().settings.left, GPIO_NUM_27, GPIO_NUM_21};

    /// Правый мотор
    Motor right_motor{Settings::storage().settings.right, GPIO_NUM_19, GPIO_NUM_18};

    /// Узел протокола Espnow
    Espnow esp_now{Settings::storage().settings.esp_now};

    /// Инициализировать всю периферию
    bool init() const {
        left_motor.init();
        right_motor.init();
        return esp_now.init();
    }
};
