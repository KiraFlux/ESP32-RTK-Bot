#pragma once

#include <Arduino.h>

#include "tools/Logger.hpp"

/// @brief Драйвер мотора
struct Motor {
  /// @brief Псевдоним типа для значения ШИМ
  using Pwm = uint16_t;

  /// @brief Настройки мотора
  struct Settings {
    /// @brief Максимальное значение ШИМ
    Pwm max_pwm;

    /// @brief Положительное направление вращения
    enum class Direction : uint8_t {
      /// @brief По часовой
      CW = 0x00,
      /// @brief Против часовой
      CCW = 0x01
    } direction;
  };

 private:
  /// @brief Настройки
  const Settings& settings;
  /// @brief Пины
  const uint8_t pin_dir, pin_speed;

 public:
  explicit constexpr Motor(const Settings& settings, gpio_num_t dir,
                           gpio_num_t speed)
      : settings{settings},
        pin_dir{static_cast<uint8_t>(dir)},
        pin_speed{static_cast<uint8_t>(speed)} {}

  /// @brief Инициализация пинов драйвера
  void init() const {
    Logger_debug("begin");

    digitalWrite(pin_dir, LOW);
    digitalWrite(pin_speed, LOW);
    pinMode(pin_dir, OUTPUT);
    pinMode(pin_speed, OUTPUT);

    Logger_debug("end");
  }

  /// @brief Установить значение ШИМ + направление
  /// @param pwm_dir Значение - ШИМ, Знак - направление
  void write(Pwm pwm_dir) const {
    constexpr auto max_pwm = 255;

    pwm_dir = constrain(pwm_dir, -max_pwm, max_pwm);
    analogWrite(pin_speed, abs(pwm_dir));
    digitalWrite(pin_dir, (pwm_dir < 0) == static_cast<uint8_t>(Settings::Direction::CW));
  }
};