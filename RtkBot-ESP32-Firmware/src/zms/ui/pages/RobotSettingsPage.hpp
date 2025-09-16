#pragma once

#include <kf/Text-UI.hpp>

#include "zms/Robot.hpp"
#include "zms/ui/pages/EncoderConversionSettingsPage.hpp"
#include "zms/ui/pages/EncoderPinsSettingsPage.hpp"
#include "zms/ui/pages/EspnowNodeSettingsPage.hpp"
#include "zms/ui/pages/MainPage.hpp"
#include "zms/ui/pages/MotorDriverSettingsPage.hpp"
#include "zms/ui/pages/MotorPwmSettingsPage.hpp"


namespace zms {

/// Страница настроек робота
struct RobotSettingsPage final : kf::Page {

private:
    /// Сохранить настройки
    kf::Button save;

    /// Загрузить настройки
    kf::Button load;

    MotorDriverSettingsPage left_motor_driver, right_motor_driver;
    MotorPwmSettingsPage motor_pwm;

    EncoderConversionSettingsPage encoder_conversion;
    EncoderPinsSettingsPage left_encoder_pins, right_encoder_pins;

    EspnowNodeSettingsPage espnow_node;

public:
    explicit RobotSettingsPage(kf::Storage<Robot::Settings> &storage) :
        Page{storage.key},
        save{"Save", [&storage](kf::Button &) { storage.save(); }},
        load{"Load", [&storage](kf::Button &) { storage.load(); }},

        left_motor_driver{"Motor Driver L", storage.settings.left_motor},
        right_motor_driver{"Motor Driver R", storage.settings.right_motor},
        motor_pwm{storage.settings.motor_pwm},

        left_encoder_pins{"Encoder Pins L", storage.settings.left_encoder},
        right_encoder_pins{"Encoder Pins R", storage.settings.right_encoder},
        encoder_conversion{storage.settings.encoder_conversion},

        espnow_node{storage.settings.espnow_node} {
        MainPage::instance().link(*this);

        add(save);
        add(load);

        link(left_motor_driver);
        link(right_motor_driver);
        link(motor_pwm);

        link(left_encoder_pins);
        link(right_encoder_pins);
        link(encoder_conversion);

        link(espnow_node);
    }
};

}// namespace zms
