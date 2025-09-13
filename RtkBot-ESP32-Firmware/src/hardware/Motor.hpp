#pragma once

#include <Arduino.h>

#include "kf/Logger.hpp"
#include "rs/aliases.hpp"


/// @brief Драйвер мотора
struct Motor {

    /// @brief Псевдоним типа для значения ШИМ
    using SignedPwm = rs::i16;

    /// @brief Настройки мотора
    struct Settings {
        /// @brief Максимальное значение ШИМ
        SignedPwm max_pwm;

        /// @brief Положительное направление вращения
        enum class Direction : rs::u8 {
            /// @brief По часовой
            CW = 0x00,
            /// @brief Против часовой
            CCW = 0x01
        } direction;
    };

private:

    /// @brief Настройки
    const Settings &settings;
    /// @brief Пины
    const rs::u8 pin_dir, pin_speed;

public:

    explicit constexpr Motor(const Settings &settings, gpio_num_t dir, gpio_num_t speed) :
        settings{settings},
        pin_dir{static_cast<rs::u8>(dir)},
        pin_speed{static_cast<rs::u8>(speed)} {}

    /// @brief Инициализация пинов драйвера
    void init() const {
        kf_Logger_info("begin");

        digitalWrite(pin_dir, LOW);
        digitalWrite(pin_speed, LOW);
        pinMode(pin_dir, OUTPUT);
        pinMode(pin_speed, OUTPUT);

        kf_Logger_debug("end");
    }

    /// @brief Установить значение ШИМ + направление
    /// @param pwm Значение - ШИМ, Знак - направление
    void write(SignedPwm pwm) const {
        pwm = constrain(pwm, -settings.max_pwm, settings.max_pwm);
        analogWrite(pin_speed, abs(pwm));
        digitalWrite(pin_dir, matchDirection(pwm));
    }

private:

    inline bool matchDirection(SignedPwm pwm) const {
        if (settings.direction == Settings::Direction::CW) {
            return pwm > 0;
        } else {
            return pwm < 0;
        }
    }
};