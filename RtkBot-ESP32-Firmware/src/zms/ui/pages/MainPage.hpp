#pragma once

#include <kf/Text-UI.hpp>

#include "zms/tools/Singleton.hpp"

/// Основная страница
struct MainPage final : kf::Page, Singleton<MainPage> {
    friend struct Singleton<MainPage>;

    explicit MainPage() :
        Page{"Main"} {}
};
