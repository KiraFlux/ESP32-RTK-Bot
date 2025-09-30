#pragma once

#include <kf/tui.hpp>

#include "zms/tools/Singleton.hpp"

namespace zms {

/// @brief Основная страница
struct MainPage final : kf::tui::Page, Singleton<MainPage> {
    friend struct Singleton<MainPage>;

    explicit MainPage() :
        kf::tui::Page{"Main"} {}
};

}// namespace zms
