#pragma once

#include <Arduino.h>


/// @brief Служба для отслеживания момента истечения допустимого таймаута
struct TimeoutManager final {

    /// @brief Псевдоним для явного обозначения миллисекунд
    using Ms = decltype(millis());

private:

    /// @brief Таймаут
    Ms timeout;
    /// @brief Время следующего таймаута
    Ms next_timeout{0};

public:

    explicit TimeoutManager(Ms timeout_duration) :
        timeout{timeout_duration} {}

    /// @brief Обновление таймаута
    void update() {
        next_timeout = millis() + timeout;
    }

    /// @brief Проверка истечения таймаута
    /// @returns <code>true</code> если таймаут просрочен
    inline bool expired() const { return millis() >= next_timeout; }
};