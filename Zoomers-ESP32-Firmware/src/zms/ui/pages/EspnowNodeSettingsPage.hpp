#pragma once

#include <kf/tui.hpp>

#include "zms/remote/EspnowNode.hpp"
#include "zms/ui/widgets/HexDisplay.hpp"

namespace zms {

/// Страница настроек узла Espnow
struct EspnowNodeSettingsPage final : kf::tui::Page {

    HexDisplay display;

    explicit EspnowNodeSettingsPage(EspnowNode::Settings &settings) :
        kf::tui::Page{"Espnow Node"},
        display{kf::tui::TextStream::Slice{
            .data = reinterpret_cast<const char *>(settings.remote_controller_mac.data()),
            .len = settings.remote_controller_mac.size()}} {
        link(MainPage::instance());
        add(display);
    }
};

}// namespace zms
