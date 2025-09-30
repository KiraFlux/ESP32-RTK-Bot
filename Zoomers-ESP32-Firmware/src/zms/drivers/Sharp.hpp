#pragma once

#include <Arduino.h>
#include <kf/Logger.hpp>

#include "zms/aliases.hpp"

namespace zms {

/// @brief ИК Датчик расстояния Sharp
struct Sharp {

    /// @brief Значение выхода АЦП
    using AnalogValue = rs::u16;

    /// @brief Настройки Sharp
    struct Settings {

        /// @brief Аналоговый пин сенсора
        rs::u8 pin;

        /// @brief Разрешение АЦП
        rs::u8 resolution;

        [[nodiscard]] bool isValid() const {
            if (resolution < 0 or resolution > 16) {
                kf_Logger_error("Invalid!");
                return false;
            }
            return true;
        }
    };

    const Settings &settings;

private:
    AnalogValue max_value{0};

public:
    explicit Sharp(const Settings &settings) :
        settings{settings} {}

    [[nodiscard]] bool init() {
        if (not settings.isValid()) { return false; }

        max_value = static_cast<AnalogValue>((1u << settings.resolution) - 1u);

        pinMode(settings.pin, INPUT);
        analogReadResolution(settings.resolution);

        return true;
    }

    /// @brief Считать значения датчика в величине АЦП 
    inline AnalogValue readRaw() const {
        return analogRead(settings.pin);
    }

    /// @brief Считать расстояние в миллиметрах
    Millimeters read() const {
        // 65535 / analogRead(a)
        long sum = 0;

        const auto n = 4;

        for (int i = 0; i < n; i += 1) {
            sum += readRaw();
            delay(1);
        }

        return (65535.0F * n) / Millimeters(sum);
    }
};

}// namespace zms
