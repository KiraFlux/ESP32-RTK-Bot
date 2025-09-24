#pragma once

#include <kf/tui.hpp>

#include "zms/drivers/Encoder.hpp"

namespace zms {

/// Страница настроек энкодера
struct EncoderConversionSettingsPage final : kf::tui::Page {


    explicit EncoderConversionSettingsPage(Encoder::ConversionSettings &settings) :
        kf::tui::Page{"Enc Conversion"} {}
};

}// namespace zms
