#pragma once

#include <Arduino.h>

#include "zms/aliases.hpp"

/// @brief Обработчик прерывания на основной фазе
static void IRAM_ATTR encoderInterruptHandler(void *);

namespace zms {

/// @brief Энкодер инкрементальный с двумя фазами
struct Encoder {

    /// @brief Псевдоним типа для положения энкодера в отсчётах (ticks)
    using Ticks = rs::i32;

    /// @brief Настройки преобразований
    struct ConversionSettings {
        /// @brief Сколько отсчётов в одном миллиметре
        rs::f32 ticks_in_one_mm;

        /// @brief Перевести из отсчётов в мм
        [[nodiscard]] Millimeters toMillimeters(Ticks ticks) const { return Millimeters(ticks) / ticks_in_one_mm; }

        /// @brief Перевести из мм в отсчёты
        [[nodiscard]] Ticks toTicks(Millimeters mm) const { return Ticks(mm * ticks_in_one_mm); }
    };

    /// @brief Настройки пинов
    struct PinsSettings {
        /// @brief Пин основного сигнала (источник прерывания)
        rs::u8 phase_a;
        /// @brief Пин вторичной фазы (для определения направления)
        rs::u8 phase_b;

        /// @brief Режим вызова прерывания
        enum class Edge : rs::u8 {
            /// @brief Прерывание по нарастанию (LOW -> HIGH)
            Rising = RISING,
            /// @brief Прерывание по спаду (HIGH -> LOW)
            Falling = FALLING
        } edge;
    };

    /// @brief Настройки подключения
    const PinsSettings &pins;
    
    /// @brief Настройки преобразования
    const ConversionSettings &conversion;
    
    /// @brief Текущее положение энкодера в отсчётах
    Ticks position{0};

    explicit Encoder(const PinsSettings &pins_settings, const ConversionSettings &conversion_settings) :
        pins{pins_settings}, conversion{conversion_settings} {}

    /// @brief Инициализировать пины энкодера
    void init() const {
        pinMode(pins.phase_a, INPUT);
        pinMode(pins.phase_b, INPUT);
    }

    /// @brief Разрешить (Подключить) обработку прерываний с основной фазы
    void enable() {
        kf_Logger_debug("ok");
        attachInterruptArg(
            pins.phase_a,
            encoderInterruptHandler,
            static_cast<void *>(this),
            static_cast<int>(pins.edge));
    }

    /// @brief Отключить обработку прерываний
    void disable() const {
        kf_Logger_debug("ok");
        detachInterrupt(pins.phase_a);
    }

    /// @brief Положение энкодера в отчётах
    [[nodiscard]] inline Ticks getPositionTicks() const { return position; }

    /// @brief Установить положение энкодера в отсчётах
    void setPositionTicks(Ticks new_position) { position = new_position; }

    /// @brief Положение энкодера в мм
    [[nodiscard]] inline Millimeters getPositionMillimeters() const { return conversion.toMillimeters(position); }

    /// @brief Установить положение энкодера в мм
    void setPositionMillimeters(Millimeters new_position) { position = conversion.toTicks(new_position); }
};

}// namespace zms

void encoderInterruptHandler(void *instance) {
    auto &encoder = *static_cast<zms::Encoder *>(instance);

    if (digitalRead(encoder.pins.phase_b)) {
        encoder.position += 1;
    } else {
        encoder.position -= 1;
    }
}