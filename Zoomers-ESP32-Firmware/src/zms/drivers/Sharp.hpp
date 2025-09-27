#pragma once

#include <Arduino.h>
#include <kf/Logger.hpp>

#include "zms/aliases.hpp"

namespace zms {

struct Sharp {

    /// Значение выхода АЦП
    using AnalogValue = rs::u16;

    struct Settings {

        /// Аналоговый пин сенсора
        rs::u8 pin;
        /// Разрешение analogRead
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
    AnalogValue max_value{0};

    explicit Sharp(const Settings &settings) :
        settings{settings} {}

    [[nodiscard]] bool init() {
        if (not settings.isValid()) { return false; }

        max_value = static_cast<AnalogValue>((1u << settings.resolution) - 1u);

        pinMode(settings.pin, INPUT);
        analogReadResolution(settings.resolution);

        return true;
    }

    inline AnalogValue readRaw() const {
        return analogRead(settings.pin);
    }

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
