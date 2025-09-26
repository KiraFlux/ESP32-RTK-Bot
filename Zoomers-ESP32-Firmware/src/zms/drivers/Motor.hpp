#pragma once

#include <Arduino.h>
#include <kf/Logger.hpp>
#include <rs/aliases.hpp>


namespace zms {

/// Драйвер мотора (IArduino Motor Shield)
struct Motor {

    /// Псевдоним типа для значения ШИМ
    using SignedPwm = rs::i16;

    /// Определяет направление положительного вращения
    enum class Direction : rs::u8 {
        /// Положительное вращение - по часовой
        CW = 0x00,
        /// Положительное вращение - против часовой
        CCW = 0x01
    };

    /// Тип драйвера мотора
    enum class DriverImpl : rs::u8 {
        IArduino = 0x00,
        L293nModule = 0x01,
    };

    /// Настройки драйвера
    struct DriverSettings {

        /// Выбранный драйвер
        DriverImpl impl;

        /// Определение положительного направления вращения
        Direction direction;

        /// IArduino Motor Shield: Пин направления (H-bridge)
        /// L293N Module: IN1 / IN3
        rs::u8 pin_a;

        /// IArduino Motor Shield: Пин скорости (Enable)
        /// L293N Module: IN2 / IN4
        rs::u8 pin_b;

        /// Канал (0 .. 15)
        rs::u8 ledc_channel;

        /// Проверяет корректность настроек
        /// @return <code>true</code> - Заданные параметры в норме
        [[nodiscard]] bool isValid() const {
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

        /// Частота ШИМ Гц
        FrequencyScalar ledc_frequency_hz;
        /// Мёртвая зона ШИМ
        SignedPwm dead_zone;
        /// Разрешение (8 .. 12)
        rs::u8 ledc_resolution_bits;

        [[nodiscard]] bool isValid() const {
            if (dead_zone < 0 or ledc_resolution_bits < 8 or ledc_resolution_bits > 12) {
                kf_Logger_error("Invalid");
                return false;
            }
            return true;
        }

        /// Рассчитать актуальное максимальное значение ШИМ
        [[nodiscard]] inline SignedPwm maxPwm() const { return static_cast<SignedPwm>((1u << ledc_resolution_bits) - 1u); }
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

    [[nodiscard]] bool init() {
        kf_Logger_info(
            "init: pins A=%d, B=%d, channel=%d\n",
            driver_settings.pin_a, driver_settings.pin_b,
            driver_settings.ledc_channel);

        if (not driver_settings.isValid() or not pwm_settings.isValid()) {
            kf_Logger_error("invalid settings!");
            return false;
        }

        max_pwm = pwm_settings.maxPwm();
        kf_Logger_debug(
            "max_pwm=%d, freq=%d, resolution=%d\n",
            max_pwm, pwm_settings.ledc_frequency_hz,
            pwm_settings.ledc_resolution_bits);

        pinMode(driver_settings.pin_a, OUTPUT);
        pinMode(driver_settings.pin_b, OUTPUT);
        kf_Logger_debug("pins configured as OUTPUT");

        switch (driver_settings.impl) {
            case DriverImpl::IArduino: {
                kf_Logger_debug("IArduino mode");
                const auto current_frequency = ledcSetup(
                    driver_settings.ledc_channel,
                    pwm_settings.ledc_frequency_hz,
                    pwm_settings.ledc_resolution_bits);

                kf_Logger_debug("LEDC setup - freq=%u", current_frequency);

                if (current_frequency == 0) {
                    kf_Logger_error("LEDC setup failed!");
                    return false;
                }

                ledcAttachPin(driver_settings.pin_b, driver_settings.ledc_channel);
                kf_Logger_debug("LEDC attached to pin");
            }
                break;

            case DriverImpl::L293nModule: {
                kf_Logger_debug("L293n mode");
                analogWriteFrequency(pwm_settings.ledc_frequency_hz);
                analogWriteResolution(pwm_settings.ledc_resolution_bits);
            }
                break;
        }

        stop();
        kf_Logger_debug("ok");

        return true;
    }

    /// Установить значение в нормализованной величине
    void set(float value) const { write(fromNormalized(value)); }

    /// Остановить мотор
    inline void stop() const { write(0); }

    /// Установить значение ШИМ + направление
    /// @param pwm Значение - ШИМ, Знак - направление
    void write(SignedPwm pwm) const {
        pwm = constrain(pwm, -max_pwm, max_pwm);

        switch (driver_settings.impl) {

            case DriverImpl::IArduino: {
                digitalWrite(driver_settings.pin_a, matchDirection(pwm));
                ledcWrite(driver_settings.ledc_channel, std::abs(pwm));
            }
                break;

            case DriverImpl::L293nModule: {
                const bool positive_direction = matchDirection(pwm);
                if (positive_direction) {
                    analogWrite(driver_settings.pin_a, std::abs(pwm));
                    analogWrite(driver_settings.pin_b, 0);
                } else {
                    analogWrite(driver_settings.pin_a, 0);
                    analogWrite(driver_settings.pin_b, std::abs(pwm));
                }
            }
                break;
        }
    }

private:
    [[nodiscard]] inline bool matchDirection(SignedPwm pwm) const {
        const bool positive = pwm > 0;
        return driver_settings.direction == Direction::CW == positive;
    }

    [[nodiscard]] SignedPwm fromNormalized(float value) const {
        if (std::isnan(value)) { return 0; }

        const auto abs_value = std::abs(constrain(value, -1.0f, +1.0f));
        if (abs_value < 1e-2f) { return 0; }

        auto ret = int(abs_value * float(max_pwm - pwm_settings.dead_zone)) + pwm_settings.dead_zone;
        if (value < 0.0f) { ret = -ret; }

        return static_cast<SignedPwm>(ret);
    }
};

}// namespace zms
