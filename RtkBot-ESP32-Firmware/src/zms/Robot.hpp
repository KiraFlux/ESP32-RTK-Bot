#pragma once

#include <kf/Storage.hpp>

#include "zms/tools/Singleton.hpp"

#include "zms/drivers/Encoder.hpp"
#include "zms/drivers/Motor.hpp"
#include "zms/remote/Espnow.hpp"

/// @brief MISIS-Zoomers
namespace zms {

/// Аппаратное обеспечение робота
struct Robot : Singleton<Robot> {
    friend struct Singleton<Robot>;

    /// Настройки аппаратного обеспечения
    struct Settings {
        /// Настройки ШИМ
        Motor::PwmSettings pwm;
        /// Настройки драйверов моторов
        Motor::DriverSettings left_motor, right_motor;

        /// Общие настройки энкодеров
        Encoder::ConvertationSettings generic_encoder;
        /// Настройки подключения энкодеров
        Encoder::PinoutSettings left_encoder, right_encoder;

        /// Настройки узла Espnow
        Espnow::Settings esp_now;
    };

    /// Хранилище настроек
    kf::Storage<Settings> storage{
        .key = "RobotSet",
        .settings = {
            .pwm = {
                .ledc_resolution_bits = 10,
                .ledc_frequency_hz = 20000,
                .dead_zone = 568,// Значение получено эксперементально
            },
            .left_motor = {
                .direction = Motor::DriverSettings::Direction::CCW,
                .direction_pin = static_cast<rs::u8>(GPIO_NUM_27),
                .speed_pin = static_cast<rs::u8>(GPIO_NUM_21),
                .ledc_channel = 0,
            },
            .right_motor = {
                .direction = Motor::DriverSettings::Direction::CW,
                .direction_pin = static_cast<rs::u8>(GPIO_NUM_19),
                .speed_pin = static_cast<rs::u8>(GPIO_NUM_18),
                .ledc_channel = 1,
            },
            .generic_encoder = {
                .ticks_in_one_mm = 1.0f,
            },
            .left_encoder = {
                .pin_a = static_cast<rs::u8>(GPIO_NUM_32),
                .pin_b = static_cast<rs::u8>(GPIO_NUM_33),
                .edge = Encoder::PinoutSettings::Edge::Rising,
            },
            .right_encoder = {
                .pin_a = static_cast<rs::u8>(GPIO_NUM_25),
                .pin_b = static_cast<rs::u8>(GPIO_NUM_26),
                .edge = Encoder::PinoutSettings::Edge::Rising,
            },
            .esp_now = {
                .remote_controller_mac = {0x78, 0x1c, 0x3c, 0xa4, 0x96, 0xdc},
            }}};

    /// Левый мотор
    Motor left_motor{storage.settings.left_motor, storage.settings.pwm};

    /// Правый мотор
    Motor right_motor{storage.settings.right_motor, storage.settings.pwm};

    /// Левый Энкодер
    Encoder left_encoder{storage.settings.left_encoder, storage.settings.generic_encoder};

    /// Правый Энкодер
    Encoder right_encoder{storage.settings.right_encoder, storage.settings.generic_encoder};

    /// Узел протокола Espnow
    Espnow esp_now{storage.settings.esp_now};

    /// Инициализировать всю периферию
    bool init() {
        // Попытка загрузить настройки
        if (not storage.load()) {
            // Не удалось - сохраняем значения по умолчанию
            if (not storage.save()) {
                return false;
            }
        }

        // Софт компоненты

        if (not esp_now.init()) { return false; }

        // Аппаратные компоненты

        if (not left_motor.init()) { return false; }
        if (not right_motor.init()) { return false; }

        left_encoder.init();
        right_encoder.init();

        return true;
    }
};

}// namespace zms
