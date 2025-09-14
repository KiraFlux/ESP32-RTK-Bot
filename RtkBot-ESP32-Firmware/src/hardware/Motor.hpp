#pragma once

#include <Arduino.h>

#include "kf/Logger.hpp"
#include "rs/aliases.hpp"


/// @brief Драйвер мотора (IArduino Motor Shield)
struct Motor {

    /// @brief Псевдоним типа для значения ШИМ
    using SignedPwm = rs::i16;

    /// @brief Настройки драйвера
    struct DriverSettings {

        /// @brief Определяет направление положительного вращения
        enum class Direction : rs::u8 {
            /// @brief Положительное вращение по часовой
            CW,
            /// @brief Положительное вращение против часовой
            CCW
        } direction;

        /// @brief Пин направления (H-bridge)
        rs::u8 driver_direction_pin;
        /// @brief Пин скорости (Enable)
        rs::u8 driver_speed_pin;
        /// @brief Канал (0 .. 15)
        rs::u8 ledc_channel;

        bool isValid() const {
            if (ledc_channel > 15) {
                kf_Logger_error("Invalid");
                return false;
            }
            return true;
        }
    };

    /// @brief Настройки ШИМ
    struct PwmSettings {
        /// @brief Разрешение (8 .. 12)
        rs::u8 ledc_resolution_bits;
        /// @brief Частота ШИМ Гц
        rs::u16 ledc_frequency_hz;
        /// @brief Мёртвая зона ШИМ
        SignedPwm dead_zone;

        bool isValid() const {
            if (dead_zone < 0 or ledc_resolution_bits < 8 or ledc_resolution_bits > 12) {
                kf_Logger_error("Invalid");
                return false;
            }
            return true;
        }

        /// @brief Рассчитать актуальной максимальное значение ШИМ
        inline SignedPwm maxPwm() const { return static_cast<SignedPwm>((1u << ledc_resolution_bits) - 1u); }
    };

    /// @brief Настройки драйвера
    const DriverSettings &driver_settings;

    /// @brief Настройки ШИМ
    const PwmSettings &pwm_settings;

private:

    /// @brief Максимальное значение ШИМ
    SignedPwm max_pwm{0};

public:

    explicit constexpr Motor(const DriverSettings &driver_settings, const PwmSettings &pwm_settings) :
        driver_settings{driver_settings}, pwm_settings(pwm_settings) {}

    /// @brief Инициализация пинов драйвера
    bool init() {
        kf_Logger_info("begin");

        if (not driver_settings.isValid() or not pwm_settings.isValid()) { return false; }

        pinMode(driver_settings.driver_direction_pin, OUTPUT);
        pinMode(driver_settings.driver_speed_pin, OUTPUT);

        const auto current_frequency = ledcSetup(
            driver_settings.ledc_channel,
            pwm_settings.ledc_frequency_hz,
            pwm_settings.ledc_resolution_bits
        );
        if (current_frequency == 0) { return false; }
        ledcAttachPin(driver_settings.driver_speed_pin, driver_settings.ledc_channel);
        stop();

        max_pwm = pwm_settings.maxPwm();

        kf_Logger_debug("end");
        return true;
    }

    /// @brief Установить значение в нормализованной величине
    void set(float value) const {
        write(fromNormalized(value));
    }

    /// @brief Установить значение ШИМ + направление
    /// @param pwm Значение - ШИМ, Знак - направление
    void write(SignedPwm pwm) const {
        pwm = constrain(pwm, -max_pwm, max_pwm);
        digitalWrite(driver_settings.driver_direction_pin, matchDirection(pwm));
        ledcWrite(driver_settings.ledc_channel, std::abs(pwm));
    }

    /// @brief Остановить мотор
    void stop() const {
        digitalWrite(driver_settings.driver_direction_pin, LOW);
        ledcWrite(driver_settings.ledc_channel, 0);
    }

private:

    inline bool matchDirection(SignedPwm pwm) const {
        const bool positive = pwm > 0;
        return driver_settings.direction == DriverSettings::Direction::CW == positive;
    }

    SignedPwm fromNormalized(float value) const {
        if (std::isnan(value)) { return 0; }

        const auto abs_value = std::abs(constrain(value, -1.0f, +1.0f));
        if (abs_value < 1e-2f) { return 0; }

        auto ret = int(abs_value * float(max_pwm - pwm_settings.dead_zone)) + pwm_settings.dead_zone;
        if (value < 0.0f) { ret = -ret; }

        return static_cast<SignedPwm>(ret);
    }

};