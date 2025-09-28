#pragma once

#include <kf/Storage.hpp>

#include "zms/tools/Singleton.hpp"

#include "zms/Task.hpp"
#include "zms/drivers/Encoder.hpp"
#include "zms/drivers/Motor.hpp"
#include "zms/drivers/Sharp.hpp"
#include "zms/remote/EspnowNode.hpp"
#include "zms/remote/EspnowRemoteController.hpp"
#include "zms/ui/pages/EncoderConversionSettingsPage.hpp"
#include "zms/ui/pages/EncoderTunePage.hpp"
#include "zms/ui/pages/EspnowNodeSettingsPage.hpp"
#include "zms/ui/pages/MainPage.hpp"
#include "zms/ui/pages/MotorPwmSettingsPage.hpp"
#include "zms/ui/pages/MotorTunePage.hpp"
#include "zms/ui/pages/TaskPage.hpp"

/// MISIS-Zoomers
namespace zms {

/// Аппаратное обеспечение робота
struct Robot : Singleton<Robot> {
    friend struct Singleton<Robot>;

    /// Настройки аппаратного обеспечения
    struct Settings {

        // Драйвер моторов

        /// Настройки ШИМ драйвера моторов
        Motor::PwmSettings motor_pwm;
        /// Настройки драйверов моторов
        Motor::DriverSettings left_motor, right_motor;

        // Энкодер

        /// Настройки преобразования энкодера
        Encoder::ConversionSettings encoder_conversion;
        /// Настройки подключения энкодеров
        Encoder::PinsSettings left_encoder, right_encoder;

        /// ИК датчики расстояния
        Sharp::Settings left_disnance_sensor, right_disnance_sensor;

        // Софт

        /// Настройки узла Espnow
        EspnowNode::Settings espnow_node;
    };

    Task task{};

    /// Хранилище настроек
    kf::Storage<Settings> storage{
        .key = "RobotSet",
        .settings = defaultSettings()};

    // Аппаратные компоненты

    /// Левый мотор
    Motor left_motor{storage.settings.left_motor, storage.settings.motor_pwm};

    /// Правый мотор
    Motor right_motor{storage.settings.right_motor, storage.settings.motor_pwm};

    /// Левый Энкодер
    Encoder left_encoder{storage.settings.left_encoder, storage.settings.encoder_conversion};

    /// Правый Энкодер
    Encoder right_encoder{storage.settings.right_encoder, storage.settings.encoder_conversion};

    /// Левый датчик расстояния
    Sharp left_disnance_sensor{storage.settings.left_disnance_sensor};

    /// Правый датчик расстояния
    Sharp right_disnance_sensor{storage.settings.right_disnance_sensor};

    //

    /// Узел протокола Espnow
    EspnowNode espnow_node{storage.settings.espnow_node};

    /// Удаленный контроллер
    EspnowRemoteController espnow_remote_controller{};

    //

    /// Инициализировать всю периферию
    [[nodiscard]] bool init() {
        // Попытка загрузить настройки
        if (not storage.load()) {
            // Не удалось - сохраняем значения по умолчанию
            if (not storage.save()) {
                return false;
            }
        }

        // Аппаратные компоненты

        if (not left_motor.init()) { return false; }
        if (not right_motor.init()) { return false; }

        if (not left_disnance_sensor.init()) { return false; }
        if (not right_disnance_sensor.init()) { return false; }

        left_encoder.init();
        right_encoder.init();

        left_encoder.enable();
        right_encoder.enable();

        // Софт компоненты

        if (not espnow_node.init()) { return false; }
        espnow_node.on_receive = Robot::onEspnowRemoteControllerPacket;

        // TUI

        setupTUI();

        const auto result = xTaskCreate(
            Robot::taskHandler,
            "robot",
            4096,
            static_cast<void *>(this),
            1,
            nullptr);

        if (result != pdPASS) {
            kf_Logger_error("Task Create fail");
            return false;
        }

        return true;
    }

    /// Прокрутка событий робота
    void poll() {
        pollTUI();
        pollRemoteControl();
    }

    [[nodiscard]] inline bool isExecutingTask() {
        return task.current_state != Task::State::Idle;
    }

    [[noreturn]] static void taskHandler(void *instance) {
        auto &robot = *static_cast<Robot *>(instance);

        while (true) {
            delay(10);

            switch (robot.task.current_state) {
                case Task::State::Idle:
                    continue;

                case Task::State::GoDist:
                    kf_Logger_debug("Execute go dist arg=%f", robot.task.arg);
                    robot.goDist(robot.task.arg);
                    break;

                default:
                    kf_Logger_warn("Invalid State: %d", int(robot.task.current_state));
                    break;
            }

            kf_Logger_info("Execute OK: %d", int(robot.task.current_state));
            robot.task.current_state = Task::State::Idle;
        }
    }

    void goDist(Millimeters distance) {
        resetEncoders();

        const auto end = std::abs(storage.settings.encoder_conversion.toTicks(distance));

        const rs::f32 speed = (distance > 0) ? task.speed : -task.speed;

        while (true) {
            const auto l = std::abs(left_encoder.getPositionTicks());
            const auto r = std::abs(right_encoder.getPositionTicks());

            const auto current = (l + r) / 2;

            if (current > end) { break; }

            syncMove(speed);

            delay(1);
        };

        setMotors(0, 0);
    }

private:
    static const Settings &defaultSettings() {
        static constexpr Settings default_settings{
            .motor_pwm = {
                .ledc_frequency_hz = 20000,
                .dead_zone = 568,// Значение получено экспериментально
                .ledc_resolution_bits = 10,
            },
            .left_motor = {
                .impl = Motor::DriverImpl::IArduino,
                .direction = Motor::Direction::CW,
                .pin_a = static_cast<rs::u8>(GPIO_NUM_27),
                .pin_b = static_cast<rs::u8>(GPIO_NUM_21),
                .ledc_channel = 0,
            },
            .right_motor = {
                .impl = Motor::DriverImpl::IArduino,
                .direction = Motor::Direction::CW,
                .pin_a = static_cast<rs::u8>(GPIO_NUM_19),
                .pin_b = static_cast<rs::u8>(GPIO_NUM_18),
                .ledc_channel = 1,
            },
            .encoder_conversion = {
                .ticks_in_one_mm = 1.0f,
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

    void resetEncoders() {
        left_encoder.position = 0;
        right_encoder.position = 0;
    }

    void setMotors(rs::f32 left_speed, rs::f32 right_speed) {
        left_speed = constrain(left_speed, -task.speed, task.speed);
        right_speed = constrain(right_speed, -task.speed, task.speed);

        left_motor.set(left_speed);
        right_motor.set(right_speed);
    }

    void syncMove(rs::f32 speed, int k = 1) {
        const auto error = left_encoder.getPositionTicks() - k * right_encoder.getPositionTicks();
        const rs::f32 p = error * 0.006;
        setMotors(speed - p, k * speed + p);
    }

    // poll

    void pollRemoteControl() {
        if (espnow_remote_controller.isPacketTimeoutExpired()) {
            if (not espnow_remote_controller.disconnected) {
                kf_Logger_info("disconnected");
                espnow_remote_controller.disconnected = true;

                espnow_remote_controller.resetControlPacket();
                left_motor.stop();
                right_motor.stop();
            }
        } else {
            espnow_remote_controller.disconnected = false;
            const auto &speed_y = espnow_remote_controller.packet.left_y;
            const auto &speed_x = espnow_remote_controller.packet.left_x;

            if (espnow_remote_controller.packet.toggle_mode) {
                const auto ax = std::abs(speed_y);
                const auto ay = std::abs(speed_x);

                if (ax + ay > 0.1) {
                    if (ax > ay) {
                        syncMove(speed_y, 1);
                    } else {
                        syncMove(speed_x, -1);
                    }
                } else {
                    left_motor.stop();
                    right_motor.stop();
                }

            } else {
                left_motor.set(speed_y + speed_x);
                right_motor.set(speed_y - speed_x);
            }
        }
    }

    // TUI

    void setupTUI() {
        auto &page_manager = kf::tui::PageManager::instance();

        // Хранилище
        {
            /// Сохранить настройки
            static kf::tui::Button save{"Save", [this](kf::tui::Button &) { storage.save(); }};

            /// Загрузить настройки
            static kf::tui::Button load{"Load", [this](kf::tui::Button &) { storage.load(); }};

            /// Восстановить значения по умолчанию
            static kf::tui::Button restore_defaults{
                "Restore", [this](kf::tui::Button &) {
                    storage.settings = defaultSettings();
                    storage.save();
                }};

            auto &p = MainPage::instance();

            p.add(save);
            p.add(load);
            p.add(restore_defaults);
        }

        // Моторы

        static TaskPage task_page{task};

        static zms::MotorTunePage left_motor_tune_page{
            "Motor L",
            left_motor,
            storage.settings.motor_pwm,
            storage.settings.left_motor};

        static zms::MotorTunePage right_motor_tune_page{
            "Motor R",
            right_motor,
            storage.settings.motor_pwm,
            storage.settings.right_motor};

        static zms::MotorPwmSettingsPage motor_pwm_settings_page{
            storage.settings.motor_pwm};

        // Энкодеры

        static zms::EncoderTunePage left_encoder_tune_page{
            "Encoder L",
            left_encoder,
            storage.settings.left_encoder};

        static zms::EncoderTunePage right_encoder_tune_page{
            "Encoder R",
            right_encoder,
            storage.settings.left_encoder};

        static zms::EncoderConversionSettingsPage encoder_conversion_settings_page{
            storage.settings.encoder_conversion};

        //

        static zms::EspnowNodeSettingsPage espnow_node_settings_page{
            storage.settings.espnow_node};

        page_manager.bind(zms::MainPage::instance());

        // Отправка актуального вида после инициализации
        sendTUI(page_manager);
    }

    void sendTUI(kf::tui::PageManager &page_manager) {
        const auto slice = page_manager.render();
        const auto result = espnow_node.send(slice.data, slice.len);

        kf_Logger_debug("Send: %d bytes", slice.len);

        if (result.fail()) { kf_Logger_error(rs::toString(result.error)); }
    }

    void pollTUI() {
        static auto &page_manager = kf::tui::PageManager::instance();

        const bool update_required = page_manager.pollEvents();
        if (update_required) { sendTUI(page_manager); }
    }

    // espnow

    static void onEspnowRemoteControllerPacket(const void *data, rs::u8 size) {

        /// Действие в меню
        enum Action : rs::u8 {
            None = 0x00,
            Reload = 0x10,
            Click = 0x20,
            Left = 0x30,
            Right = 0x31,
            Up = 0x40,
            Down = 0x41
        };

        auto translate = [](Action code) {
            using kf::tui::Event;

            switch (code) {
                case Action::Reload: return Event::Update;
                case Action::Click: return Event::Click;
                case Action::Left: return Event::ChangeIncrement;
                case Action::Right: return Event::ChangeDecrement;
                case Action::Up: return Event::ElementPrevious;
                case Action::Down: return Event::ElementNext;
                case Action::None:
                default: return Event::None;
            }
        };

        static auto &espnow_remote_controller = Robot::instance().espnow_remote_controller;
        static auto &page_manager = kf::tui::PageManager::instance();

        switch (size) {
            case sizeof(EspnowRemoteController::ControlPacket)://
                espnow_remote_controller.updateControlPacket(*static_cast<const EspnowRemoteController::ControlPacket *>(data));
                return;

            case sizeof(Action)://
                page_manager.addEvent(translate(*static_cast<const Action *>(data)));
                return;

            default: kf_Logger_warn("Unknown packet: (%d bytes)", size);
        }
    }
};

}// namespace zms
