#pragma once

#include <kf/Text-UI.hpp>

#include "zms/drivers/motor/Motor.hpp"
#include "zms/ui/widgets/ComboBox.hpp"

namespace zms {

/// Страница настроек драйверов моторов
struct MotorDriverSettingsPage final : kf::Page {

    /// Направление движения
    ComboBox<Motor::DriverSettings::Direction, 2> direction;

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
