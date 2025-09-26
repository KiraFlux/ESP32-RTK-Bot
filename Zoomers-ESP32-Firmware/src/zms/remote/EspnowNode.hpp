#pragma once

#include <WiFi.h>

#include <espnow/Mac.hpp>
#include <espnow/Protocol.hpp>

#include <kf/Logger.hpp>


namespace zms {

/// Узел Espnow
struct EspnowNode {

public:
    /// Настройки узла
    struct Settings {
        /// MAC-Адрес пульта
        espnow::Mac remote_controller_mac;
    };

private:
    /// Настройки узла
    const Settings &settings;

public:
    /// Обработчик входящего пакета от пульта
    std::function<void(const void *, rs::u8)> on_receive{nullptr};

    explicit EspnowNode(const Settings &settings) :
        settings{settings} {}

    /// Инициализировать протокол
    /// @returns <code>true</code> - Успешная инициализация
    [[nodiscard]] bool init() const {
        kf_Logger_info("init");

        const bool wifi_ok = WiFiClass::mode(WIFI_MODE_STA);
        if (not wifi_ok) {
            return false;
        }

        const auto init_result = espnow::Protocol::init();
        if (init_result.fail()) {
            kf_Logger_error(rs::toString(init_result.error));
            return false;
        }

        const auto peer_result = espnow::Peer::add(settings.remote_controller_mac);
        if (peer_result.fail()) {
            kf_Logger_error(rs::toString(peer_result.error));
            return false;
        }

        auto receive_handler = [this](const espnow::Mac &mac, const void *data, rs::u8 len) {
            if (this->on_receive and mac == this->settings.remote_controller_mac) { this->on_receive(data, len); }
        };
        const auto handler_result = espnow::Protocol::instance().setReceiveHandler(receive_handler);
        if (handler_result.fail()) {
            kf_Logger_error(rs::toString(handler_result.error));
            return false;
        }

        kf_Logger_debug("success");
        return true;
    }

    /// Отправить пакет данных на пульт
    template<typename T> inline rs::Result<void, espnow::Protocol::SendError> send(const T &value) {
        return espnow::Protocol::send(settings.remote_controller_mac, value);
    }

    /// Отправить буфер на пульт
    inline rs::Result<void, espnow::Protocol::SendError> send(const void *data, rs::u8 size) {
        return espnow::Protocol::send(settings.remote_controller_mac, data, size);
    }
};

}// namespace zms
