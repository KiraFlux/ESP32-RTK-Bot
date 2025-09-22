#pragma once

#include <Arduino.h>

#include "zms/aliases.hpp"

namespace zms {

/// Таймер
struct Timer {

    /// Период срабатывания таймера
    Microseconds period;

private:
    Microseconds last{0};

public:
    explicit Timer(Milliseconds period) :
        period{period} {}

    /// Прошел ли период
    bool ready() {
        const auto now = millis();
        const auto delta = now - last;

        if (delta < period) { return false; }

        last = now;

        return true;
    }
};

}// namespace zms
