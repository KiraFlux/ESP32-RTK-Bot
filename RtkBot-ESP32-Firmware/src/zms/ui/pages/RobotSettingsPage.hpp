#pragma once

#include <kf/Text-UI.hpp>

#include "zms/Robot.hpp"
#include "zms/ui/pages/MainPage.hpp"

/// Страница настроек
struct RobotSettingsPage final : kf::Page {
    kf::Button save_button;

    explicit RobotSettingsPage(kf::Storage<Robot::Settings> &storage) :
        Page{storage.key},
        save_button{"save", [&storage](kf::Button &) { storage.save(); }} {
        MainPage::instance().link(*this);
        add(save_button);
    }
};
