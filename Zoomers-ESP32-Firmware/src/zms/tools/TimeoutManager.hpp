#pragma once

#include <Arduino.h>

#include "zms/aliases.hpp"

namespace zms {

/// @brief Служба для отслеживания момента истечения допустимого таймаута
struct TimeoutManager final {

private:
    /// @brief Таймаут
    Milliseconds timeout;

    /// @brief Время следующего таймаута
    Milliseconds next_timeout{0};

public:
    explicit TimeoutManager(Milliseconds timeout_duration) :
        timeout{timeout_duration} {}

    /// @brief Обновление таймаута
    void update() {
        next_timeout = millis() + timeout;
    }

    /// @brief Проверка истечения таймаута
    /// @returns <code>true</code> если таймаут просрочен
    [[nodiscard]] inline bool expired() const { return millis() >= next_timeout; }
};

}// namespace zms
