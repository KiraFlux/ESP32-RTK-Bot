#pragma once

#include <kf/Text-UI.hpp>

#include "zms/drivers/Encoder.hpp"

namespace zms {

/// Страница настроек подключения энкодеров
struct EncoderPinoutSettingsPage final : kf::Page {
    EncoderPinoutSettingsPage(const char *encoder_name, Encoder::PinoutSettings &settings) :
        Page{encoder_name} {}
};

}// namespace zms
