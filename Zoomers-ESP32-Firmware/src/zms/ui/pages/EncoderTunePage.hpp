#pragma once

#include <kf/Logger.hpp>
#include <kf/Text-UI.hpp>

#include "zms/drivers/Encoder.hpp"
#include "zms/ui/pages/MainPage.hpp"
#include "zms/ui/widgets/CheckBox.hpp"

namespace zms {

struct EncoderTunePage final : kf::Page {

    using TicksDisplay = kf::Labeled<kf::Display<Encoder::Ticks>>;

    TicksDisplay ticks_display;
    kf::Labeled<CheckBox> enabled;

    explicit EncoderTunePage(const char *encoder_name, Encoder &encoder) :
        Page{encoder_name},
        ticks_display{
            "Pos",
            TicksDisplay::Content{encoder.position}},
        enabled{
            "Enabled",
            CheckBox{[&encoder, encoder_name](bool e) {
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
