#pragma once

#include <Arduino.h>

/// Служба для отслеживания момента истечения допустимого таймаута
struct TimeoutManager final {

    /// Псевдоним для явного обозначения миллисекунд
    using Ms = decltype(millis());

private:
    /// Таймаут
    Ms timeout;
    /// Время следующего таймаута
    Ms next_timeout{0};

public:
    explicit TimeoutManager(Ms timeout_duration) :
        timeout{timeout_duration} {}

    /// Обновление таймаута
    void update() {
        next_timeout = millis() + timeout;
    }

    /// Проверка истечения таймаута
    /// @returns <code>true</code> если таймаут просрочен
    inline bool expired() const { return millis() >= next_timeout; }
};