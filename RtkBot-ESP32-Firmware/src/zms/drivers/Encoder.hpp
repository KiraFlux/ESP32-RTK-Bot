#pragma once

#include <Arduino.h>
#include <rs/aliases.hpp>

namespace zms {

/// Энкодер инкреметальный с двумя фазами
struct Encoder {

    /// Псевдоним типа для положения энкодера в отсчётах (ticks)
    using Ticks = rs::i32;

    /// Псевдоним типа для положения энкодера в миллиметрах (mm)
    using Millimeters = rs::f64;

    /// Настройки преобразований
    struct ConvertationSettings {
        /// Сколько отсчёток в одном миллиметре
        rs::f32 ticks_in_one_mm;

        /// Перевести из отсчётов в мм
        Millimeters toMillimeters(Ticks ticks) const { return Millimeters(ticks) / ticks_in_one_mm; }

        /// Перевести из мм в отсчёты
        Ticks toTicks(Millimeters mm) const { return Ticks(mm * ticks_in_one_mm); }
    };

    /// Настройки пинов
    struct PinoutSettings {
        /// Основная фаза (Фаза на прерывании)
        rs::u8 pin_a;
        /// Вторая фаза (Фаза направления)
        rs::u8 pin_b;

        /// Режим вызова прерывания
        enum class Edge : rs::u8 {
            /// Вызов прерывания на подъёме (LOW -> HIGH)
            Rising = RISING,
            /// Вызов прерывания на спаде (HIGH -> LOW)
            Falling = FALLING
        } edge;
    };

    /// Настройки подключения
    const PinoutSettings &pinout_settings;
    /// Общие настройки
    const ConvertationSettings &converation_settings;

private:
    /// Текущее положение энкодера в отсчётах
    Ticks position{0};

public:
    explicit Encoder(const PinoutSettings &pinout_settings, const ConvertationSettings &generic_settings) :
        pinout_settings{pinout_settings}, converation_settings{generic_settings} {}

    /// Инициализировать пины энкодера
    void init() const {
        pinMode(pinout_settings.pin_a, INPUT);
        pinMode(pinout_settings.pin_b, INPUT);
    }

    /// Разрешить (Подключить) обработку прерываний с основной фазы
    void enable() {
        attachInterruptArg(
            pinout_settings.pin_a,
            Encoder::InterruptHandler,
            static_cast<void *>(this),
            static_cast<int>(pinout_settings.edge));
    }

    /// Отключить обработку прерываний
    void disable() const {
        detachInterrupt(pinout_settings.pin_a);
    }

    /// Положение энкодера в отчётах
    inline Ticks getPositionTicks() const { return position; }

    /// Установить положение энкодера в отсчётах
    void setPositionTicks(Ticks new_positon) { position = new_positon; }

    /// Положение энкодера в мм
    inline Millimeters getPositionMillimeters() const { return converation_settings.toMillimeters(position); }

    /// Установить положение энкодера в мм
    void setPositionMillimeters(Millimeters new_positon) { position = converation_settings.toTicks(new_positon); }

private:
    /// Обработчик прерывания на основной фазе
    static void IRAM_ATTR InterruptHandler(void *instance) {
        auto &encoder = *static_cast<Encoder *>(instance);

        if (digitalRead(encoder.pinout_settings.pin_b)) {
            encoder.position += 1;
        } else {
            encoder.position -= 1;
        }
    }
};

}// namespace zms
