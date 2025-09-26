#pragma once

#include <kf/Logger.hpp>
#include <kf/tui.hpp>

#include "zms/drivers/Encoder.hpp"
#include "zms/ui/pages/MainPage.hpp"

namespace zms {

struct EncoderTunePage final : kf::tui::Page {

    using TicksDisplay = kf::tui::Labeled<kf::tui::Display<Encoder::Ticks>>;

    TicksDisplay ticks_display;
    kf::tui::Labeled<kf::tui::CheckBox> enabled;
    kf::tui::ComboBox<Encoder::PinsSettings::Edge, 2> edge;

    explicit EncoderTunePage(
        const char *encoder_name,
        Encoder &encoder,
        Encoder::PinsSettings &settings) :
        kf::tui::Page{encoder_name},
        ticks_display{
            "Pos",
            TicksDisplay::Content{encoder.position}},
        enabled{
            "Enabled",
            kf::tui::CheckBox{[&encoder, encoder_name](bool e) {
                kf_Logger_info("%s: %s", encoder_name, e ? "Enabled" : "Disabled");
                if (e) {
                    encoder.enable();
                } else {
                    encoder.disable();
                }
            }}},
        edge{
            {{
                {"Rising", Encoder::PinsSettings::Edge::Rising},
                {"Falling", Encoder::PinsSettings::Edge::Falling},
            }},
            settings.edge} {
        link(MainPage::instance());
        add(ticks_display);
        add(enabled);
        add(edge);
    }
};

}// namespace zms
