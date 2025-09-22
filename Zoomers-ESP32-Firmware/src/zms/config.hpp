#pragma once

/// Глобальная конфигурация ZMS
namespace zms::config {

/// Тип драйвера мотора
enum class MotorDriver {
    IArduino,
    L293nModule,
};

//constexpr auto selected_motor_driver = MotorDriver::IArduino;
constexpr auto selected_motor_driver = MotorDriver::L293nModule;

}// namespace zms::config
