#pragma once

#include <Arduino.h>
#include <rs/aliases.hpp>

/// Incremental Two-Phase Encoder
struct Encoder {

    /// Type Alias for Encoder position
    using Tick = rs::i32;

    /// Type Alias for position in mm
    using Mm = rs::f64;

    /// Generic (Common) Encoder settings
    struct GenericSettings {
        /// Ticks to mm convertation constant
        rs::f32 ticks_in_one_mm;

        Mm toMm(Tick ticks) const { return Mm(ticks) / ticks_in_one_mm; }
        Tick toTicks(Mm mm) const { return Tick(mm * ticks_in_one_mm); }
    };

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
    const GenericSettings &generic_settings;

private:
    /// Current Encoder position
    Tick position{0};

public:
    explicit Encoder(const PinoutSettings &pinout_settings, const GenericSettings &generic_settings) :
        pinout_settings{pinout_settings}, generic_settings{generic_settings} {}

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

    /// Get current Encoder position in Ticks
    inline Tick positionTicks() const { return position; }

    /// Set current Encoder position in Ticks
    void setPositionTicks(Tick new_positon) { position = new_positon; }

    /// Get current Encoder position in mm
    inline Mm positionMm() const { return generic_settings.toMm(position); }

    /// Set current Encoder position in Ticks
    void setPositionMm(Mm new_positon) { position = generic_settings.toTicks(new_positon); }

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