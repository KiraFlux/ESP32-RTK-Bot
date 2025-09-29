#pragma once

#include <kf/Storage.hpp>

#include "zms/drivers/Encoder.hpp"
#include "zms/drivers/EspnowNode.hpp"
#include "zms/drivers/Motor.hpp"
#include "zms/drivers/Sharp.hpp"
#include "zms/tools/Singleton.hpp"

/// @brief MISIS-Zoomers
namespace zms {

/// @brief Аппаратное обеспечение робота.
/// Предоставляет доступ к аппаратным модулям робота и их настройкам.
/// Может быть только единственным
struct Periphery final : Singleton<Periphery> {
    friend struct Singleton<Periphery>;

    /// @brief Настройки аппаратного обеспечения
    struct Settings {

        // Драйвер моторов

        /// @brief Настройки ШИМ драйвера моторов
        Motor::PwmSettings motor_pwm;
        /// @brief Настройки драйверов моторов
        Motor::DriverSettings left_motor, right_motor;

        // Энкодер

        /// @brief Настройки преобразования энкодера
        Encoder::ConversionSettings encoder_conversion;
        /// @brief Настройки подключения энкодеров
        Encoder::PinsSettings left_encoder, right_encoder;

        /// @brief ИК датчики расстояния
        Sharp::Settings left_disnance_sensor, right_disnance_sensor;

        // Софт

        /// @brief Настройки узла Espnow
        EspnowNode::Settings espnow_node;
    };

    /// @brief Хранилище настроек
    kf::Storage<Settings> storage{.key = "RobotSet", .settings = defaultSettings()};

    // Аппаратные компоненты

    /// @brief Левый мотор
    Motor left_motor{storage.settings.left_motor, storage.settings.motor_pwm};

    /// @brief Правый мотор
    Motor right_motor{storage.settings.right_motor, storage.settings.motor_pwm};

    /// @brief Левый Энкодер
    Encoder left_encoder{storage.settings.left_encoder, storage.settings.encoder_conversion};

    /// @brief Правый Энкодер
    Encoder right_encoder{storage.settings.right_encoder, storage.settings.encoder_conversion};

    /// @brief Левый датчик расстояния
    Sharp left_disnance_sensor{storage.settings.left_disnance_sensor};

    /// @brief Правый датчик расстояния
    Sharp right_disnance_sensor{storage.settings.right_disnance_sensor};

    /// @brief Узел протокола Espnow
    EspnowNode espnow_node{storage.settings.espnow_node};

    /// @brief Инициализировать всю периферию
    [[nodiscard]] bool init() {
        // Попытка загрузить настройки
        if (not storage.load()) {
            // Не удалось - сохраняем значения по умолчанию
            if (not storage.save()) {
                return false;
            }
        }

        if (not left_motor.init()) { return false; }
        if (not right_motor.init()) { return false; }

        if (not left_disnance_sensor.init()) { return false; }
        if (not right_disnance_sensor.init()) { return false; }

        left_encoder.init();
        right_encoder.init();

        left_encoder.enable();
        right_encoder.enable();

        if (not espnow_node.init()) { return false; }

        return true;
    }

    /// @brief Получть настройки по умолчанию
    /// @return Значения по умолчанию (Из прошивки)
    static const Settings &defaultSettings() {
        static constexpr Settings default_settings{
            .motor_pwm = {
                .ledc_frequency_hz = 20000,
                .dead_zone = 300,// Значение получено экспериментально
                .ledc_resolution_bits = 10,
            },
            .left_motor = {
                .impl = Motor::DriverImpl::L293nModule,
                .direction = Motor::Direction::CCW,
                .pin_a = static_cast<rs::u8>(GPIO_NUM_27),
                .pin_b = static_cast<rs::u8>(GPIO_NUM_21),
                .ledc_channel = 0,
            },
            .right_motor = {
                .impl = Motor::DriverImpl::L293nModule,
                .direction = Motor::Direction::CCW,
                .pin_a = static_cast<rs::u8>(GPIO_NUM_19),
                .pin_b = static_cast<rs::u8>(GPIO_NUM_18),
                .ledc_channel = 1,
            },
            .encoder_conversion = {
                .ticks_in_one_mm = (5000.0f / 2100.0f),
            },
            .left_encoder = {
                .phase_a = static_cast<rs::u8>(GPIO_NUM_32),
                .phase_b = static_cast<rs::u8>(GPIO_NUM_33),
                .edge = Encoder::PinsSettings::Edge::Rising,
            },
            .right_encoder = {
                .phase_a = static_cast<rs::u8>(GPIO_NUM_25),
                .phase_b = static_cast<rs::u8>(GPIO_NUM_26),
                .edge = Encoder::PinsSettings::Edge::Falling,
            },
            .left_disnance_sensor = {
                .pin = static_cast<rs::u8>(GPIO_NUM_34),
                .resolution = 10,
            },
            .right_disnance_sensor = {
                .pin = static_cast<rs::u8>(GPIO_NUM_35),
                .resolution = 10,
            },
            .espnow_node = {
                .remote_controller_mac = {0x78, 0x1c, 0x3c, 0xa4, 0x96, 0xdc},
            }};
        return default_settings;
    }
};

}// namespace zms
