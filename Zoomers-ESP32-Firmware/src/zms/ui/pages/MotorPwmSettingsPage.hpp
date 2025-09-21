#pragma once

#include <kf/Text-UI.hpp>

#include "zms/drivers/motor/Motor.hpp"

namespace zms {

/// Страница настроек ШИМ
struct MotorPwmSettingsPage final : kf::Page {
    explicit MotorPwmSettingsPage(Motor::PwmSettings &settings) :
        Page{"Motor PWM"} {}
};

}// namespace zms
