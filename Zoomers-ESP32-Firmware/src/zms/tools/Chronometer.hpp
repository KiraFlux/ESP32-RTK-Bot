#pragma once

#include <Arduino.h>

#include "zms/aliases.hpp"

namespace zms {

/// @brief Хронометр
struct Chronometer final {

private:
    /// @brief Время предыдущего измерения
    Microseconds last_us{micros()};

public:
    /// @brief Рассчитать дельту между вызовами
    /// @returns dt Сек.
    Seconds calc() noexcept {
        const auto current_us = micros();
        const auto delta_us = current_us - last_us;
        last_us = current_us;
        return static_cast<decltype(calc())>(delta_us) * 1e-6f;
    }
};

}// namespace zms
