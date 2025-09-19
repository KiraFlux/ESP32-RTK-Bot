#pragma once

#include <Arduino.h>

#include "zms/aliases.hpp"

namespace zms {

/// Энкодер инкрементальный с двумя фазами
struct Encoder {

    /// Псевдоним типа для положения энкодера в отсчётах (ticks)
    using Ticks = rs::i32;

    /// Настройки преобразований
    struct ConversionSettings {
        /// Сколько отсчётов в одном миллиметре
        rs::f32 ticks_in_one_mm;

        /// Перевести из отсчётов в мм
        Millimeters toMillimeters(Ticks ticks) const { return Millimeters(ticks) / ticks_in_one_mm; }

        /// Перевести из мм в отсчёты
        Ticks toTicks(Millimeters mm) const { return Ticks(mm * ticks_in_one_mm); }
    };

    /// Настройки пинов
    struct PinsSettings {
        /// Пин основного сигнала (источник прерывания)
        rs::u8 phase_a;
        /// Пин вторичной фазы (для определения направления)
        rs::u8 phase_b;

        /// Режим вызова прерывания
        enum class Edge : rs::u8 {
            /// Прерывание по нарастанию (LOW -> HIGH)
            Rising = RISING,
            /// Прерывание по спаду (HIGH -> LOW)
            Falling = FALLING
        } edge;
    };

    /// Настройки подключения
    const PinsSettings &pins;
    /// Настройки преобразования
    const ConversionSettings &conversion;
    /// Текущее положение энкодера в отсчётах
    Ticks position{0};

    explicit Encoder(const PinsSettings &pins_settings, const ConversionSettings &conversion_settings) :
        pins{pins_settings}, conversion{conversion_settings} {}

    /// Инициализировать пины энкодера
    void init() const {
        pinMode(pins.phase_a, INPUT);
        pinMode(pins.phase_b, INPUT);
    }

    /// Разрешить (Подключить) обработку прерываний с основной фазы
    void enable() {
        attachInterruptArg(
            pins.phase_a,
            Encoder::InterruptHandler,
            static_cast<void *>(this),
            static_cast<int>(pins.edge));
    }

    /// Отключить обработку прерываний
    void disable() const {
        detachInterrupt(pins.phase_a);
    }

    /// Положение энкодера в отчётах
    inline Ticks getPositionTicks() const { return position; }

    /// Установить положение энкодера в отсчётах
    void setPositionTicks(Ticks new_position) { position = new_position; }

    /// Положение энкодера в мм
    inline Millimeters getPositionMillimeters() const { return conversion.toMillimeters(position); }

    /// Установить положение энкодера в мм
    void setPositionMillimeters(Millimeters new_position) { position = conversion.toTicks(new_position); }

private:
    /// Обработчик прерывания на основной фазе
    static void IRAM_ATTR InterruptHandler(void *instance) {
        auto &encoder = *static_cast<Encoder *>(instance);

        if (digitalRead(encoder.pins.phase_b)) {
            encoder.position += 1;
        } else {
            encoder.position -= 1;
        }
    }
};

}// namespace zms
