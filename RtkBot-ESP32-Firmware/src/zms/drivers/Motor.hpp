#pragma once

#include <Arduino.h>
#include <kf/Logger.hpp>
#include <rs/aliases.hpp>

namespace zms {

/// Драйвер мотора (IArduino Motor Shield)
struct Motor {

    /// Псевдоним типа для значения ШИМ
    using SignedPwm = rs::i16;

    /// Настройки драйвера
    struct DriverSettings {

        /// Определяет направление положительного вращения
        enum class Direction : rs::u8 {
            /// Положительное вращение по часовой
            CW,
            /// Положительное вращение против часовой
            CCW
        } direction;

        /// Пин направления (H-bridge)
        rs::u8 direction_pin;
        /// Пин скорости (Enable)
        rs::u8 speed_pin;
        /// Канал (0 .. 15)
        rs::u8 ledc_channel;

        /// Проверяет корректность настроек
        /// @return <code>true</code> - Заданные параметры в норме
        bool isValid() const {
            if (ledc_channel > 15) {
                kf_Logger_error("Invalid");
                return false;
            }
            return true;
        }
    };

    /// Настройки ШИМ
    struct PwmSettings {
        using FrequencyScalar = rs::u16;

        /// Разрешение (8 .. 12)
        rs::u8 ledc_resolution_bits;
        /// Частота ШИМ Гц
        FrequencyScalar ledc_frequency_hz;
        /// Мёртвая зона ШИМ
        SignedPwm dead_zone;

        bool isValid() const {
            if (dead_zone < 0 or ledc_resolution_bits < 8 or ledc_resolution_bits > 12) {
                kf_Logger_error("Invalid");
                return false;
            }
            return true;
        }

        /// Рассчитать актуальное максимальное значение ШИМ
        inline SignedPwm maxPwm() const { return static_cast<SignedPwm>((1u << ledc_resolution_bits) - 1u); }
    };

    /// Настройки драйвера
    const DriverSettings &driver_settings;

    /// Настройки ШИМ
    const PwmSettings &pwm_settings;

private:
    /// Максимальное значение ШИМ
    SignedPwm max_pwm{0};

public:
    explicit constexpr Motor(const DriverSettings &driver_settings, const PwmSettings &pwm_settings) :
        driver_settings{driver_settings}, pwm_settings(pwm_settings) {}

    /// Инициализация пинов драйвера
    bool init() {
        kf_Logger_info("begin");

        if (not driver_settings.isValid() or not pwm_settings.isValid()) { return false; }

        pinMode(driver_settings.direction_pin, OUTPUT);
        pinMode(driver_settings.speed_pin, OUTPUT);

        const auto current_frequency = ledcSetup(
            driver_settings.ledc_channel,
            pwm_settings.ledc_frequency_hz,
            pwm_settings.ledc_resolution_bits);
        if (current_frequency == 0) { return false; }
        ledcAttachPin(driver_settings.speed_pin, driver_settings.ledc_channel);
        stop();

        max_pwm = pwm_settings.maxPwm();

        kf_Logger_debug("end");
        return true;
    }

    /// Установить значение в нормализованной величине
    void set(float value) const {
        write(fromNormalized(value));
    }

    /// Установить значение ШИМ + направление
    /// @param pwm Значение - ШИМ, Знак - направление
    void write(SignedPwm pwm) const {
        pwm = constrain(pwm, -max_pwm, max_pwm);
        digitalWrite(driver_settings.direction_pin, matchDirection(pwm));
        ledcWrite(driver_settings.ledc_channel, std::abs(pwm));
    }

    /// Остановить мотор
    void stop() const {
        digitalWrite(driver_settings.direction_pin, LOW);
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

}// namespace zms
