#pragma once

#include "tools/Singleton.hpp"

#include "hardware/Motor.hpp"
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

        /// Получить ссылку на Singleton экземпляр хранилища настроек
        static kf::Storage<Settings> &storage() {
            static kf::Storage<Settings> instance{
                .key="r-set",
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

    /// Инициализировать всю периферию
    void init() const {
        left_motor.init();
        right_motor.init();
    }
};
