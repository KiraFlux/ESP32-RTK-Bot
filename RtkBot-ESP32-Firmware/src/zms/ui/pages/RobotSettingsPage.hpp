#pragma once

#include <kf/Text-UI.hpp>

#include "zms/Robot.hpp"
#include "zms/ui/pages/EncoderConvertaionSettingsPage.hpp"
#include "zms/ui/pages/EncoderPinoutSettingsPage.hpp"
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

    EncoderConvertaionSettingsPage encoder_convertation;

    EncoderPinoutSettingsPage left_encoder_pinout, right_encoder_pinout;

    EspnowNodeSettingsPage espnow_node;

public:
    explicit RobotSettingsPage(kf::Storage<Robot::Settings> &storage) :
        Page{storage.key},
        save{"Save", [&storage](kf::Button &) { storage.save(); }},
        load{"Load", [&storage](kf::Button &) { storage.load(); }},

        left_motor_driver{"Motor Driver L", storage.settings.left_motor},
        right_motor_driver{"Motor Driver R", storage.settings.right_motor},
        motor_pwm{storage.settings.pwm},

        left_encoder_pinout{"Encoder Pinout L", storage.settings.left_encoder},
        right_encoder_pinout{"Encoder Pinout R", storage.settings.right_encoder},
        encoder_convertation{storage.settings.encoder_convertation},

        espnow_node{storage.settings.espnow_node}

    {
        MainPage::instance().link(*this);
        add(save);

        link(left_motor_driver);
        link(right_motor_driver);
        link(motor_pwm);

        link(left_encoder_pinout);
        link(right_encoder_pinout);
        link(encoder_convertation);

        link(espnow_node);
    }
};

}// namespace zms
