#pragma once

#include <functional>
#include <kf/Logger.hpp>
#include <kf/tui.hpp>

#include "zms/drivers/Periphery.hpp"

#include "zms/ui/pages/EncoderConversionSettingsPage.hpp"
#include "zms/ui/pages/EncoderTunePage.hpp"
#include "zms/ui/pages/EspnowNodeSettingsPage.hpp"
#include "zms/ui/pages/MainPage.hpp"
#include "zms/ui/pages/MotorPwmSettingsPage.hpp"
#include "zms/ui/pages/MotorTunePage.hpp"
#include "zms/ui/pages/StoragePage.hpp"

namespace zms {

/// @brief ZMS Text UI
struct TextUI final {

    /// @brief Обработчик отправки TUI
    std::function<bool(const kf::tui::TextStream::Slice &)> send_handler{nullptr};

    /// @brief Страница управления хранилищем настроек
    StoragePage storage_page;

    /// @brief Страница настройки драйверов моторов
    MotorTunePage left_motor_tune_page, right_motor_tune_page;
    /// @brief Страница настройки ШИМ драйвера моторов
    MotorPwmSettingsPage motor_pwm_settings_page;

    /// @brief Страница настройки энкодеров
    EncoderTunePage left_encoder_tune_page, right_encoder_tune_page;
    /// @brief Страница настройки конвертации энкодеров
    EncoderConversionSettingsPage encoder_conversion_settings_page;

    /// @brief Страница настройки узла Espnow
    EspnowNodeSettingsPage espnow_node_settings_page;

    explicit TextUI(Periphery &periphery) :
        left_motor_tune_page{
            "Motor L",
            periphery.left_motor,
            periphery.storage.settings.motor_pwm,
            periphery.storage.settings.left_motor},

        right_motor_tune_page{
            "Motor R",
            periphery.right_motor,
            periphery.storage.settings.motor_pwm,
            periphery.storage.settings.right_motor},

        motor_pwm_settings_page{
            periphery.storage.settings.motor_pwm},

        left_encoder_tune_page{
            "Encoder L",
            periphery.left_encoder,
            periphery.storage.settings.left_encoder},

        right_encoder_tune_page{
            "Encoder R",
            periphery.right_encoder,
            periphery.storage.settings.left_encoder},

        encoder_conversion_settings_page{
            periphery.storage.settings.encoder_conversion},

        espnow_node_settings_page{
            periphery.storage.settings.espnow_node} {

        auto &page_manager = kf::tui::PageManager::instance();
        page_manager.bind(MainPage::instance());
    }

    /// @brief Опрос TUI
    void poll() {
        if (not send_handler) { return; }

        auto &page_manager = kf::tui::PageManager::instance();
        const bool update_required = page_manager.pollEvents();

        if (not update_required) { return; }

        const auto slice = page_manager.render();
        const auto result = send_handler(slice);

        if (not result) {
            kf_Logger_error("send failed");
            return;
        }

        kf_Logger_debug("%d bytes send", slice.len);
    }
};

}// namespace zms
