#pragma once

#include <Arduino.h>
#include <rs/aliases.hpp>

/// Incremental Two-Phase Encoder
struct Encoder {

    /// Type Alias for Encoder position
    using Tick = rs::i32;

    /// Encoder pinout settings
    struct PinoutSettings {
        /// Main (Int) pin
        rs::u8 pin_phase_a;
        /// Secondary (Dir) pin
        rs::u8 pin_phase_b;

        /// Encoder Interrupt Mode
        enum class Mode : rs::u8 {
            Rising = RISING,
            Falling = FALLING
        } mode;
    };

    const PinoutSettings &pinout_settings;
    /// Current Encoder position
    Tick position{0};

    explicit Encoder(const PinoutSettings &pinout_settings) :
        pinout_settings{pinout_settings} {}

    /// Initialize Encoder pins
    void init() const {
        pinMode(pinout_settings.pin_phase_a, INPUT);
        pinMode(pinout_settings.pin_phase_b, INPUT);
    }

    /// Enable Interrupt
    void enable() {
        attachInterruptArg(
            pinout_settings.pin_phase_a,
            Encoder::onMainPhaseInt,
            static_cast<void *>(this),
            static_cast<int>(pinout_settings.mode));
    }

    /// Disable Interrupt
    void disable() const {
        detachInterrupt(pinout_settings.pin_phase_a);
    }

private:
    /// On Main phase interrupt handler
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