#pragma once

#include <kf/Text-UI.hpp>

#include "zms/drivers/Encoder.hpp"

namespace zms {

/// Страница настроек энкодера
struct EncoderConvertaionSettingsPage final : kf::Page {
    EncoderConvertaionSettingsPage(Encoder::ConvertationSettings &settings) :
        Page{"Enc Convertation"} {}
};

}// namespace zms
