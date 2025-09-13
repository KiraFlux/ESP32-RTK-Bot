#include <Arduino.h>

#include "tools/Logger.hpp"
#include "tools/Storage.hpp"

#include "hardware/Motor.hpp"

static Storage<Motor::Settings> left_motor_storage{
    .key = "lms",
    .settings = {.max_pwm = 255, .direction = Motor::Settings::Direction::CCW}};

static Storage<Motor::Settings> right_motor_storage{
    .key = "rms",
    .settings = {.max_pwm = 255, .direction = Motor::Settings::Direction::CW}};

static const Motor left_motor{left_motor_storage.settings, GPIO_NUM_27,
                              GPIO_NUM_21};
static const Motor right_motor{right_motor_storage.settings, GPIO_NUM_19,
                               GPIO_NUM_18};

void setup() {
  Logger_info("begin");

  Serial.begin(115200);

  Logger::instance().write_func = [](const char* text, size_t lenght) {
    Serial.write(text, lenght);
  };

  left_motor.init();
  right_motor.init();

  Logger_debug("init ok");

  delay(2000);
}

void loop() {
  delay(5000);
}
