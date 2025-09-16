#pragma once

#include <kf/Text-UI.hpp>

#include "zms/remote/Espnow.hpp"

namespace zms {

/// Страница настроек узла Espnow
struct EspnowNodeSettingsPage final : kf::Page {
    explicit EspnowNodeSettingsPage(EspnowNode::Settings &settings) :
        Page{"Espnow Node"} {}
};

}// namespace zms
