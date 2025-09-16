#pragma once

#include <kf/Text-UI.hpp>

#include "zms/drivers/Motor.hpp"

namespace zms {

/// Страница настроек драйверов моторов
struct MotorDriverSettingsPage final : kf::Page {
    MotorDriverSettingsPage(const char *motor_name, Motor::DriverSettings &settings) :
        Page{motor_name} {}
};

}// namespace zms
