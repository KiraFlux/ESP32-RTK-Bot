#pragma once

#include <kf/tui.hpp>

#include "zms/drivers/Encoder.hpp"

namespace zms {

/// Страница настроек энкодера
struct EncoderConversionSettingsPage final : kf::Page {


    explicit EncoderConversionSettingsPage(Encoder::ConversionSettings &settings) :
        Page{"Enc Conversion"} {}
};

}// namespace zms
