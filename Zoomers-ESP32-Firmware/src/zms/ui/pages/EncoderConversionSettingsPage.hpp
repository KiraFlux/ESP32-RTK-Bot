#pragma once

#include <kf/tui.hpp>

#include "zms/drivers/Encoder.hpp"
#include "zms/ui/pages/MainPage.hpp"

namespace zms {

/// Страница настроек энкодера
struct EncoderConversionSettingsPage final : kf::tui::Page {

    explicit EncoderConversionSettingsPage(Encoder::ConversionSettings &settings) :
        kf::tui::Page{"Enc Conversion"} {
        link(MainPage::instance());
    }
};

}// namespace zms
