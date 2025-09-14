#pragma once

#include "tools/Singleton.hpp"

#include "hardware/Motor.hpp"
#include "hardware/Espnow.hpp"
#include "kf/Storage.hpp"


/// @brief Аппаратное обеспечение робота
struct Robot : Singleton<Robot> {
    friend struct Singleton<Robot>;

    /// @brief Настройки аппаратного обеспечения
    struct Settings {
        /// @brief Настройки ШИМ
        Motor::PwmSettings pwm_settings;
        /// @brief Настройки драйверов моторов
        Motor::DriverSettings left_motor, right_motor;
        /// @brief Настройки узла Espnow
        Espnow::Settings esp_now;
    };

    /// Хранилище настроек
    kf::Storage<Settings> storage{
        .key="RobotSet",
        .settings={
            .pwm_settings={
                .ledc_resolution_bits=10,
                .ledc_frequency_hz=20000,
                .dead_zone=568,
            },
            .left_motor={
                .direction=Motor::DriverSettings::Direction::CCW,
                .driver_direction_pin=static_cast<rs::u8>(GPIO_NUM_27),
                .driver_speed_pin=static_cast<rs::u8>(GPIO_NUM_21),
                .ledc_channel=0,
            },
            .right_motor={
                .direction=Motor::DriverSettings::Direction::CW,
                .driver_direction_pin=static_cast<rs::u8>(GPIO_NUM_19),
                .driver_speed_pin=static_cast<rs::u8>(GPIO_NUM_18),
                .ledc_channel=1,
            },
            .esp_now={
                .remote_controller_mac={0x78, 0x1c, 0x3c, 0xa4, 0x96, 0xdc}
            }
        }
    };

    /// Левый мотор
    Motor left_motor{storage.settings.left_motor, storage.settings.pwm_settings};

    /// Правый мотор
    Motor right_motor{storage.settings.right_motor, storage.settings.pwm_settings};

    /// Узел протокола Espnow
    Espnow esp_now{storage.settings.esp_now};

    /// Инициализировать всю периферию
    bool init() {
        if (not storage.load() and not storage.save()) { return false; }
        if (not left_motor.init()) { return false; }
        if (not right_motor.init()) { return false; }
        if (not esp_now.init()) { return false; }
        return true;
    }
};
