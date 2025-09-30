#pragma once

#include <Arduino.h>
#include <rs/aliases.hpp>

#include "zms/tools/LowFrequencyFilter.hpp"

namespace zms {

/// @brief Реализация PID регулятора с внешней зависимостью настроек
struct PID {

public:
    /// @brief Настройки регулятора
    struct Settings {
        /// @brief Коэффициент компонент регулятора
        rs::f32 p, i, d;

        /// @brief Ограничение значение интеграла
        rs::f32 i_limit;

        /// @brief Ограничение выходного значения
        rs::f32 output_limit;
    };

private:
    const Settings &settings;
    LowFrequencyFilter<rs::f32> dx_filter;
    rs::f32 dx{0};
    rs::f32 ix{0};
    rs::f32 last_error{NAN};

public:
    explicit PID(const Settings &settings, rs::f32 dx_filter_alpha = 1.0f) :
        settings{settings}, dx_filter{dx_filter_alpha} {}

    rs::f32 calc(rs::f32 error, rs::f32 dt) {
        constexpr auto max_dt = 0.1f;

        if (dt <= 0.0f or dt > max_dt) {
            return 0.0f;
        }

        if (settings.i != 0.0f) {
            ix += error * dt;
            ix = constrain(ix, -settings.i_limit, settings.i_limit);
        }

        if (settings.d != 0.0f and not std::isnan(last_error)) {
            dx = dx_filter.calc((error - last_error) / dt);
        } else {
            dx = 0.0f;
        }
        last_error = error;

        const auto output = settings.p * error + settings.i * ix + settings.d * dx;
        return constrain(output, -settings.output_limit, settings.output_limit);
    }

    void reset() {
        dx = 0.0f;
        ix = 0.0f;
        last_error = NAN;
    }
};

}// namespace zms
