#pragma once

namespace zms {

/// Глобальная конфигурация ZMS
namespace config {

/// Тип драйвера мотора
enum class MotorDriver {
    IArduino,
    L293nModule,
};

constexpr auto selected_motor_driver = MotorDriver::IArduino;

}// namespace config
}// namespace zms
