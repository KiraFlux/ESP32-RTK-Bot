#pragma once

#include <kf/tui.hpp>

#include "zms/drivers/Motor.hpp"

namespace zms {

/// Страница настроек драйверов моторов
struct MotorDriverSettingsPage final : kf::tui::Page {

    /// Направление движения
    kf::tui::ComboBox<Motor::Direction, 2> direction;

    MotorDriverSettingsPage(const char *motor_name, Motor::DriverSettings &settings) :
        Page{motor_name},
        direction{
            {{
                {"CW", Motor::Direction::CW},
                {"CCW", Motor::Direction::CCW},
            }},
            settings.direction} {
        add(direction);
    }
};

}// namespace zms
