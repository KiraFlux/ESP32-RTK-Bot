#pragma once

#include <Arduino.h>
#include <rs/aliases.hpp>

/// @brief Incremental Two-Phase Encoder
struct Encoder {

    /// @brief Type Alias for Encoder position
    using Tick = rs::i32;

    /// @brief Encoder pinout settings
    struct PinoutSettings {
        /// @brief Main (Int) pin
        rs::u8 pin_phase_a;
        /// @brief Secondary (Dir) pin
        rs::u8 pin_phase_b;

        /// @brief Encoder Interrupt Mode
        enum class Mode : rs::u8 {
            Rising = RISING,
            Falling = FALLING
        } mode;
    };

    const PinoutSettings &pinout_settings;
    /// @brief Current Encoder position
    Tick position{0};

    explicit Encoder(const PinoutSettings &pinout_settings) :
        pinout_settings{pinout_settings} {}

    /// @brief Initialize Encoder pins
    void init() const {
        pinMode(pinout_settings.pin_phase_a, INPUT);
        pinMode(pinout_settings.pin_phase_b, INPUT);
    }

    /// @brief Enable Interrupt
    void enable() {
        attachInterruptArg(
            pinout_settings.pin_phase_a,
            Encoder::onMainPhaseInt,
            static_cast<void *>(this),
            static_cast<int>(pinout_settings.mode));
    }

    /// @brief Disable Interrupt
    void disable() const {
        detachInterrupt(pinout_settings.pin_phase_a);
    }

private:
    /// @brief On Main phase interrupt handler
    /// @param v Encoder Instance ptr
    static void onMainPhaseInt(void *v) {
        auto &encoder = *static_cast<Encoder *>(v);

        if (digitalRead(encoder.pinout_settings.pin_phase_b)) {
            encoder.position += 1;
        } else {
            encoder.position -= 1;
        }
    }
};