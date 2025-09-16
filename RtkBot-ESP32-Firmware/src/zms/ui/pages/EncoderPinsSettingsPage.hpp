#pragma once

#include <kf/Text-UI.hpp>

#include "zms/drivers/Encoder.hpp"


namespace zms {

/// Страница настроек подключения энкодеров
struct EncoderPinsSettingsPage final : kf::Page {
    explicit EncoderPinsSettingsPage(const char *encoder_name, Encoder::PinsSettings &settings) :
        Page{encoder_name} {}
};

}// namespace zms
