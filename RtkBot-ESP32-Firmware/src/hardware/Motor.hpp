#pragma once

#include <Arduino.h>

#include "kf/Logger.hpp"
#include "rs/aliases.hpp"


/// @brief Драйвер мотора (IArduino Motor Shield)
struct Motor {

    /// @brief Псевдоним типа для значения ШИМ
    using SignedPwm = rs::i16;

    /// @brief Настройки мотора
    struct Settings {

        /// @brief Определяет направление положительного вращения
        enum class Direction : rs::u8 {
            /// @brief Положительное вращение по часовой
            CW,
            /// @brief Положительное вращение против часовой
            CCW
        } direction;

        // Драйвер

        /// @brief Пин направления (H-bridge)
        rs::u8 driver_direction_pin;
        /// @brief Пин скорости (Enable)
        rs::u8 driver_speed_pin;

        // ledc

        /// @brief Канал (0 .. 15)
        rs::u8 ledc_channel;
        /// @brief Разрешение (8 .. 12)
        rs::u8 ledc_resolution_bits;
        /// @brief Частота ШИМ Гц
        rs::u16 ledc_frequency_hz;

        /// @brief Рассчитать актуальной максимальное значение ШИМ
        inline SignedPwm maxPwm() const { return static_cast<SignedPwm>((1u << ledc_resolution_bits) - 1u); }
    };

    /// @brief Настройки
    const Settings &settings;

private:

    /// @brief Максимальное значение ШИМ
    SignedPwm max_pwm{0};

public:

    explicit constexpr Motor(const Settings &settings) :
        settings{settings} {}

    /// @brief Инициализация пинов драйвера
    bool init() {
        kf_Logger_info("begin");

        if (settings.ledc_resolution_bits < 1 or settings.ledc_resolution_bits > 16) { return false; }

        pinMode(settings.driver_direction_pin, OUTPUT);
        pinMode(settings.driver_speed_pin, OUTPUT);

        const auto current_frequency = ledcSetup(
            settings.ledc_channel,
            settings.ledc_frequency_hz,
            settings.ledc_resolution_bits
        );
        if (current_frequency == 0) { return false; }
        ledcAttachPin(settings.driver_speed_pin, settings.ledc_channel);
        ledcWrite(settings.ledc_channel, 0);

        max_pwm = settings.maxPwm();

        kf_Logger_debug("end");
        return true;
    }

    /// @brief Установить значение ШИМ + направление
    /// @param pwm Значение - ШИМ, Знак - направление
    void write(SignedPwm pwm) const {
        pwm = constrain(pwm, -max_pwm, max_pwm);
        digitalWrite(settings.driver_direction_pin, matchDirection(pwm));
        ledcWrite(settings.ledc_channel, std::abs(pwm));
    }

private:

    inline bool matchDirection(SignedPwm pwm) const {
        // todo Проверить валидность
        const bool positive = pwm > 0;
        return settings.direction == Settings::Direction::CW == positive;
    }

};