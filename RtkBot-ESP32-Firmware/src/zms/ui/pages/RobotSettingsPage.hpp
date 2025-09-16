#pragma once

#include <kf/Text-UI.hpp>

#include "zms/Robot.hpp"
#include "zms/ui/pages/MainPage.hpp"

namespace zms {

/// Страница настроек
struct RobotSettingsPage final : kf::Page {

private:

    /// Записать настройки
    kf::Button save;

    /// Страница настроек драйверов моторов
    struct MotorDriver final : kf::Page {
        MotorDriver(const char *motor_name, Motor::DriverSettings &settings) :
            Page{motor_name} {}
    } left_motor_driver, right_motor_driver;

    /// Страница настроек ШИМ
    struct MotorPwm final : kf::Page {
        MotorPwm(Motor::PwmSettings &settings) :
            Page{"Motor PWM"} {}
    } motor_pwm;

    /// Страница настроек энкодера
    struct EncoderConvertaion final : kf::Page {
        EncoderConvertaion(Encoder::ConvertationSettings &settings) :
            Page{"Enc Convertation"} {}
    } encoder_convertation;

    /// Страница настроек подключения энкодеров
    struct EncoderPinout final : kf::Page {
        EncoderPinout(const char *encoder_name, Encoder::PinoutSettings &settings) :
            Page{encoder_name} {}
    } left_encoder_pinout, right_encoder_pinout;

    /// Страница настроек узла Espnow
    struct Espnow final : kf::Page {
        Espnow(EspnowNode::Settings &settings) :
            Page{"Espnow Node"} {}
    } esp_now;

public:
    explicit RobotSettingsPage(kf::Storage<Robot::Settings> &storage) :
        Page{storage.key},
        save{"save", [&storage](kf::Button &) { storage.save(); }},

        left_motor_driver{"Motor Driver L", storage.settings.left_motor},
        right_motor_driver{"Motor Driver R", storage.settings.right_motor},
        motor_pwm{storage.settings.pwm},

        left_encoder_pinout{"Encoder Pinout L", storage.settings.left_encoder},
        right_encoder_pinout{"Encoder Pinout R", storage.settings.right_encoder},
        encoder_convertation{storage.settings.generic_encoder},

        esp_now{storage.settings.esp_now}

    {
        MainPage::instance().link(*this);
        add(save);

        link(left_motor_driver);
        link(right_motor_driver);
        link(motor_pwm);

        link(left_encoder_pinout);
        link(right_encoder_pinout);
        link(encoder_convertation);
    }
};

}// namespace zms
