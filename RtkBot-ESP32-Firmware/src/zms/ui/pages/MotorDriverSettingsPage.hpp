#pragma once

#include <kf/Text-UI.hpp>

#include "zms/ui/widgets/CheckBox.hpp"
#include "zms/drivers/Motor.hpp"


namespace zms {

/// Страница настроек драйверов моторов
struct MotorDriverSettingsPage final : kf::Page {

    /// Направление движения
    CheckBox<Motor::DriverSettings::Direction, 2> direction;

    MotorDriverSettingsPage(const char *motor_name, Motor::DriverSettings &settings) :
        Page{motor_name},
        direction{
            {
                {
                    {"CW", Motor::DriverSettings::Direction::CW},
                    {"CCW", Motor::DriverSettings::Direction::CCW},
                }
            },
            settings.direction
        } {
        add(direction);
    }
};

}// namespace zms
