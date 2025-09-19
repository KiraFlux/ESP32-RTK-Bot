#pragma once

#include <kf/Text-UI.hpp>

#include "zms/remote/Espnow.hpp"
#include "zms/ui/widgets/HexDisplay.hpp"

namespace zms {

/// Страница настроек узла Espnow
struct EspnowNodeSettingsPage final : kf::Page {

    HexDisplay display;

    explicit EspnowNodeSettingsPage(EspnowNode::Settings &settings) :
        Page{"Espnow Node"},
        display{kf::TextStream::Slice{
            .data = reinterpret_cast<const char *>(settings.remote_controller_mac.data()),
            .len = settings.remote_controller_mac.size()}} {
        add(display);
    }
};

}// namespace zms
