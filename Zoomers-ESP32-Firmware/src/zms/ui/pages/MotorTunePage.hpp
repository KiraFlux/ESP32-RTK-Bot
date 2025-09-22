#pragma once

#include <kf/tui.hpp>
#include <rs/aliases.hpp>

#include "zms/Robot.hpp"
#include "zms/ui/pages/MainPage.hpp"
#include "zms/ui/widgets/EventObserver.hpp"


namespace zms {

/// Страница настройки моторов
struct MotorTunePage final : kf::Page {
    kf::Button set_current_pwm_as_dead_zone;
    kf::Button re_init;

    using PwmInput = EventObserver<kf::Labeled<kf::SpinBox<Motor::SignedPwm>>>;
    using PwmDuty = PwmInput::Observable::Content::Scalar;
    using PwmStepInput = kf::Labeled<kf::SpinBox<PwmDuty>>;

    PwmInput pwm_input;
    PwmStepInput pwm_step_input;
    PwmDuty pwm_step;
    PwmDuty current_pwm{0};
    const PwmDuty pwm_step_step{2};

    using NormalizedInput = EventObserver<kf::Labeled<kf::SpinBox<rs::f32>>>;
    using NormalizedValue = NormalizedInput::Observable::Content::Scalar;
    using NormalizedValueStepInput = kf::Labeled<kf::SpinBox<NormalizedValue>>;

    NormalizedInput normalized_input;
    NormalizedValueStepInput normalized_value_step_input;
    NormalizedValue normalized_value_step{0.1f};
    NormalizedValue current_normalized_value{0.0f};
    const NormalizedValue normalized_value_step_step{0.1f};

    using FrequencyInput = kf::Labeled<kf::SpinBox<Motor::PwmSettings::FrequencyScalar>>;
    FrequencyInput frequency_input;
    const Motor::PwmSettings::FrequencyScalar frequency_step{2500};

    explicit MotorTunePage(const char *title, Motor &motor, kf::Storage<Robot::Settings> &storage) :
        Page{title},
        pwm_step{static_cast<PwmDuty>(1u << (motor.pwm_settings.ledc_resolution_bits - 2))},
        set_current_pwm_as_dead_zone{
            "Set DeadZone",
            [this, &storage](kf::Button &) {
                storage.settings.motor_pwm.dead_zone = current_pwm;
            }
        },
        re_init{
            "Re-Init",
            [&motor](kf::Button &) {
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
            [this, &motor](kf::Event) {
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
            [this, &motor](kf::Event) {
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
                storage.settings.motor_pwm.ledc_frequency_hz,
                frequency_step,
                FrequencyInput::Content::Mode::ArithmeticPositiveOnly
            }
        } {
        MainPage::instance().link(*this);
        add(pwm_input);
        add(pwm_step_input);
        add(set_current_pwm_as_dead_zone);
        add(normalized_input);
        add(normalized_value_step_input);
        add(frequency_input);
        add(re_init);
    }
};

}// namespace zms
