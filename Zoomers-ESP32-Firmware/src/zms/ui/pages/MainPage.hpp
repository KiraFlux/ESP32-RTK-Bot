#pragma once

#include <kf/tui.hpp>

#include "zms/tools/Singleton.hpp"

namespace zms {

/// Основная страница
struct MainPage final : kf::Page, Singleton<MainPage> {
    friend struct Singleton<MainPage>;

    explicit MainPage() :
        Page{"Main"} {}
};

}// namespace zms
