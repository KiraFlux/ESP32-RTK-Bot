#pragma once

#include <kf/tui.hpp>

#include "zms/Robot.hpp"
#include "zms/ui/pages/MainPage.hpp"

namespace zms {

/// Страница настроек робота
struct StoragePage final : kf::tui::Page {

private:
    /// Сохранить настройки
    kf::tui::Button save;
    /// Загрузить настройки
    kf::tui::Button load;

public:
    explicit StoragePage(kf::Storage<Robot::Settings> &storage) :
        Page{storage.key},
        save{"Save", [&storage](kf::tui::Button &) { storage.save(); }},
        load{"Load", [&storage](kf::tui::Button &) { storage.load(); }}
    {
        link(MainPage::instance());
        add(save);
        add(load);
    }
};

}// namespace zms
