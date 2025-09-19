#pragma once

#include <Arduino.h>

#include "zms/aliases.hpp"

namespace zms {

/// Служба для отслеживания момента истечения допустимого таймаута
struct TimeoutManager final {

private:
    /// Таймаут
    Milliseconds timeout;
    /// Время следующего таймаута
    Milliseconds next_timeout{0};

public:
    explicit TimeoutManager(Milliseconds timeout_duration) :
        timeout{timeout_duration} {}

    /// Обновление таймаута
    void update() {
        next_timeout = millis() + timeout;
    }

    /// Проверка истечения таймаута
    /// @returns <code>true</code> если таймаут просрочен
    inline bool expired() const { return millis() >= next_timeout; }
};

}// namespace zms
