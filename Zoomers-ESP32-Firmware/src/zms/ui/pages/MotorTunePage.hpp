#pragma once

#include <kf/tui.hpp>
#include <rs/aliases.hpp>

#include "zms/Robot.hpp"
#include "zms/ui/pages/MainPage.hpp"
#include "zms/ui/widgets/EventObserver.hpp"


namespace zms {

/// Страница настройки моторов
struct MotorTunePage final : kf::tui::Page {
    kf::tui::Button set_current_pwm_as_dead_zone;
    kf::tui::Button re_init;

    using PwmInput = EventObserver<kf::tui::Labeled<kf::tui::SpinBox<Motor::SignedPwm>>>;
    using PwmDuty = PwmInput::Observable::Content::Scalar;
    using PwmStepInput = kf::tui::Labeled<kf::tui::SpinBox<PwmDuty>>;

    PwmInput pwm_input;
    PwmStepInput pwm_step_input;
    PwmDuty pwm_step;
    PwmDuty current_pwm{0};
    const PwmDuty pwm_step_step{2};

    using NormalizedInput = EventObserver<kf::tui::Labeled<kf::tui::SpinBox<rs::f32>>>;
    using NormalizedValue = NormalizedInput::Observable::Content::Scalar;
    using NormalizedValueStepInput = kf::tui::Labeled<kf::tui::SpinBox<NormalizedValue>>;

    NormalizedInput normalized_input;
    NormalizedValueStepInput normalized_value_step_input;
    NormalizedValue normalized_value_step{0.1f};
    NormalizedValue current_normalized_value{0.0f};
    const NormalizedValue normalized_value_step_step{0.1f};

    using FrequencyInput = kf::tui::Labeled<kf::tui::SpinBox<Motor::PwmSettings::FrequencyScalar>>;
    FrequencyInput frequency_input;
    const Motor::PwmSettings::FrequencyScalar frequency_step{2500};

    /// Направление движения
    kf::tui::ComboBox<Motor::Direction, 2> direction;

    kf::tui::ComboBox<Motor::DriverImpl, 2> driver_impl;

    explicit MotorTunePage(
        const char *motor_name,
        Motor &motor,
        Motor::PwmSettings &pwm_settings,
        Motor::DriverSettings &driver_settings

    ) :
        Page{motor_name},
        pwm_step{static_cast<PwmDuty>(1u << (motor.pwm_settings.ledc_resolution_bits - 2))},
        set_current_pwm_as_dead_zone{
            "Set DeadZone",
            [this, &pwm_settings](kf::tui::Button &) {
                pwm_settings.dead_zone = current_pwm;
            }
        },
        re_init{
            "Re-Init",
            [&motor](kf::tui::Button &) {
                motor.init();
            }
        },
        pwm_input{
            PwmInput::Observable{
                "PWM",
                PwmInput::Observable::Content{
                    current_pwm,
                    pwm_step
                }
            },
            [this, &motor](kf::tui::Event) {
                motor.write(current_pwm);
            }
        },
        pwm_step_input{
            "Step",
            PwmStepInput::Content{
                pwm_step,
                pwm_step_step,
                PwmStepInput::Content::Mode::Geometric
            }
        },
        normalized_input{
            NormalizedInput::Observable{
                "Norm",
                NormalizedInput::Observable::Content{
                    current_normalized_value,
                    normalized_value_step
                }
            },
            [this, &motor](kf::tui::Event) {
                motor.set(current_normalized_value);
            }
        },
        normalized_value_step_input{
            "Step",
            NormalizedValueStepInput::Content{
                normalized_value_step,
                normalized_value_step_step,
                NormalizedValueStepInput::Content::Mode::Geometric
            }
        },
        frequency_input{
            "Hz",
            FrequencyInput::Content{
                pwm_settings.ledc_frequency_hz,
                frequency_step,
                FrequencyInput::Content::Mode::ArithmeticPositiveOnly
            }
        },
        direction{
            {
                {
                    {"CW", Motor::Direction::CW},
                    {"CCW", Motor::Direction::CCW},
                }
            },
            driver_settings.direction
        },
        driver_impl{
            {
                {
                    {"IArduino", Motor::DriverImpl::IArduino},
                    {"L293N", Motor::DriverImpl::L293nModule},
                }
            },
            driver_settings.impl
        } {
        link(MainPage::instance());

        add(pwm_input);
        add(pwm_step_input);
        add(set_current_pwm_as_dead_zone);

        add(normalized_input);
        add(normalized_value_step_input);
        add(direction);

        add(re_init);
        add(frequency_input);
        add(driver_impl);
    }
};

}// namespace zms
