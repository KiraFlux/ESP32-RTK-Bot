#pragma once

#include "zms/tools/Singleton.hpp"

#include "zms/drivers/Periphery.hpp"
#include "zms/remote/ByteLangBridgeProtocol.hpp"
#include "zms/remote/EspnowRemoteController.hpp"
#include "zms/ui/TextUI.hpp"

namespace zms {

/// @brief Единственная и глобальная точках входа и связывания всех систем робота
struct Robot final : Singleton<Robot> {
    friend struct Singleton<Robot>;

    /// @brief Аппаратное обеспечение робота
    Periphery periphery{};

    /// @brief Менеджер текстового пользоввательского интерфейса
    TextUI text_ui;

    /// @brief Удаленный контроллер
    EspnowRemoteController espnow_remote_controller{};
};

}// namespace zms
