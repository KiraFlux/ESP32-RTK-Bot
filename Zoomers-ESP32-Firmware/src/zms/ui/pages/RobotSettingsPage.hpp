#pragma once

#include <kf/tui.hpp>

#include "zms/Robot.hpp"
#include "zms/ui/pages/EncoderConversionSettingsPage.hpp"
#include "zms/ui/pages/EncoderPinsSettingsPage.hpp"
#include "zms/ui/pages/EspnowNodeSettingsPage.hpp"
#include "zms/ui/pages/MainPage.hpp"
#include "zms/ui/pages/MotorPwmSettingsPage.hpp"

namespace zms {

/// Страница настроек робота
struct RobotSettingsPage final : kf::tui::Page {

private:
    /// Сохранить настройки
    kf::tui::Button save;

    /// Загрузить настройки
    kf::tui::Button load;

public:
    MotorPwmSettingsPage motor_pwm;

    EncoderConversionSettingsPage encoder_conversion;
    EncoderPinsSettingsPage left_encoder_pins, right_encoder_pins;

    EspnowNodeSettingsPage espnow_node;

    explicit RobotSettingsPage(kf::Storage<Robot::Settings> &storage) :
        Page{storage.key},
        save{"Save", [&storage](kf::tui::Button &) { storage.save(); }},
        load{"Load", [&storage](kf::tui::Button &) { storage.load(); }},

        motor_pwm{storage.settings.motor_pwm},

        left_encoder_pins{"Encoder Pins L", storage.settings.left_encoder},
        right_encoder_pins{"Encoder Pins R", storage.settings.right_encoder},
        encoder_conversion{storage.settings.encoder_conversion},

        espnow_node{storage.settings.espnow_node} {
        MainPage::instance().link(*this);

        add(save);
        add(load);

        link(motor_pwm);

        link(left_encoder_pins);
        link(right_encoder_pins);
        link(encoder_conversion);

        link(espnow_node);
    }
};

}// namespace zms
