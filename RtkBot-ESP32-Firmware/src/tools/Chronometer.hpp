#pragma once

#include <Arduino.h>


/// Хронометр
struct Chronometer final {

private:

    /// Время предыдущего измерения
    decltype(micros()) last_us{micros()};

public:

    /// Рассчитать дельту между вызовами
    /// @returns dt Сек.
    float calc() noexcept {
        const auto current_us = micros();
        const auto delta_us = current_us - last_us;
        last_us = current_us;
        return static_cast<decltype(calc())>(delta_us) * 1e-6f;
    }
};