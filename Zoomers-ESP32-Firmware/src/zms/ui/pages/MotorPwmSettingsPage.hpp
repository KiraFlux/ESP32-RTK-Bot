#pragma once

#include <kf/tui.hpp>

#include "zms/drivers/Motor.hpp"

namespace zms {

/// Страница настроек ШИМ
struct MotorPwmSettingsPage final : kf::Page {
    explicit MotorPwmSettingsPage(Motor::PwmSettings &settings) :
        Page{"Motor PWM"} {}
};

}// namespace zms
