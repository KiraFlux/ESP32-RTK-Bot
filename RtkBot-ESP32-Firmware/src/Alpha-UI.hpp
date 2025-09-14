#pragma once

#include "kf/Text-UI.hpp"
#include "hardware/Robot.hpp"


/// @brief Основная страница
struct MainPage final : kf::Page, Singleton<MainPage> {
    friend struct Singleton<MainPage>;

    explicit MainPage() :
        Page{"Main"} {}
};

/// @brief Страница настроек
struct RobotSettingsPage final : kf::Page {
    kf::Button save_button;

    explicit RobotSettingsPage(kf::Storage<Robot::Settings> &storage) :
        Page{storage.key},
        save_button{"save", [&storage](kf::Button &) { storage.save(); }} {
        MainPage::instance().link(*this);
        add(save_button);
    }
};

/// @brief Страница тестирования моторов
struct TestMotorPage final : kf::Page {
    using Input = kf::Labeled<kf::SpinBox<Motor::SignedPwm>>;

    Input pwm_input;
    kf::Button set_button;
    Input::Content::Scalar pwm{0};
    const Input::Content::Scalar step{128};

    explicit TestMotorPage(const char *title, const Motor &motor) :
        Page{title},
        set_button{"write", [&motor, this](kf::Button &) { motor.write(this->pwm); }},
        pwm_input{"pwm", Input::Content{pwm, step}} {
        MainPage::instance().link(*this);
        add(set_button);
        add(pwm_input);
    }
};