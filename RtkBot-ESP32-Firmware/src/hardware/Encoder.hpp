#pragma once

#include <Arduino.h>
#include <rs/aliases.hpp>

/// Энкодер Инкреметальный с двумя
struct Encoder {

    /// Псевдоним типа для положения энкодера в отсчётах
    using Tick = rs::i32;

    /// Псевдоним типа для положения энкодера в мм
    using Mm = rs::f64;

    /// Общие настройки (Настройки преобразований)
    struct GenericSettings {
        /// Из отсчётов в мм
        rs::f32 ticks_in_one_mm;

        Mm toMm(Tick ticks) const { return Mm(ticks) / ticks_in_one_mm; }
        Tick toTicks(Mm mm) const { return Tick(mm * ticks_in_one_mm); }
    };

    /// Настройки пинов
    struct PinoutSettings {
        /// Основная фаза (Фаза на прерывании)
        rs::u8 pin_phase_a;
        /// Вторая фаза (Фаза направления)
        rs::u8 pin_phase_b;

        /// Режим вызова прерывания
        enum class Mode : rs::u8 {
            Rising = RISING,
            Falling = FALLING
        } mode;
    };

    const PinoutSettings &pinout_settings;
    const GenericSettings &generic_settings;

private:
    /// Текущее положение энкодера в отсчётах
    Tick position{0};

public:
    explicit Encoder(const PinoutSettings &pinout_settings, const GenericSettings &generic_settings) :
        pinout_settings{pinout_settings}, generic_settings{generic_settings} {}

    /// Инициализировать пины энкодера
    void init() const {
        pinMode(pinout_settings.pin_phase_a, INPUT);
        pinMode(pinout_settings.pin_phase_b, INPUT);
    }

    /// Разрешить (Подключить) обработку прерываний с основной фазы
    void enable() {
        attachInterruptArg(
            pinout_settings.pin_phase_a,
            Encoder::mainPhaseIntrruptHandler,
            static_cast<void *>(this),
            static_cast<int>(pinout_settings.mode));
    }

    /// Отключить обработку прерываний 
    void disable() const {
        detachInterrupt(pinout_settings.pin_phase_a);
    }

    /// Положение энкодера в отчётах
    inline Tick positionTicks() const { return position; }

    /// Установить положение энкодера в отсчётах
    void setPositionTicks(Tick new_positon) { position = new_positon; }

    /// Положение энкодера в мм
    inline Mm positionMm() const { return generic_settings.toMm(position); }

    /// Установить положение энкодера в мм
    void setPositionMm(Mm new_positon) { position = generic_settings.toTicks(new_positon); }

private:
    /// Обработчик прерывания на основной фазе
    /// @param v Указатель на экземпляр Encoder
    static void IRAM_ATTR mainPhaseIntrruptHandler(void *v) {
        auto &encoder = *static_cast<Encoder *>(v);

        if (digitalRead(encoder.pinout_settings.pin_phase_b)) {
            encoder.position += 1;
        } else {
            encoder.position -= 1;
        }
    }
};