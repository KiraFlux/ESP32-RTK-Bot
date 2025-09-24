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

    explicit EncoderTunePage(const char *encoder_name, Encoder &encoder) :
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
            }}} {
        MainPage::instance().link(*this);
        add(ticks_display);
        add(enabled);
    }
};

}// namespace zms
