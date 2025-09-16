#pragma once

#include <kf/Text-UI.hpp>

#include "zms/drivers/Motor.hpp"

namespace zms {

/// Страница настроек ШИМ
struct MotorPwmSettingsPage final : kf::Page {
    MotorPwmSettingsPage(Motor::PwmSettings &settings) :
        Page{"Motor PWM"} {}
};

}// namespace zms
