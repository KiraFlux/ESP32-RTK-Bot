#pragma once

#include <kf/Text-UI.hpp>

#include "zms/ui/widgets/ComboBox.hpp"
#include "zms/drivers/Encoder.hpp"


namespace zms {

/// Страница настроек подключения энкодеров
struct EncoderPinsSettingsPage final : kf::Page {

    ComboBox<Encoder::PinsSettings::Edge, 2> edge;

    explicit EncoderPinsSettingsPage(const char *encoder_name, Encoder::PinsSettings &settings) :
        Page{encoder_name}, edge{
        {
            {
                {"Rising", Encoder::PinsSettings::Edge::Rising},
                {"Falling", Encoder::PinsSettings::Edge::Falling},
            }
        },
        settings.edge
    } {
        add(edge);
    }
};

}// namespace zms
