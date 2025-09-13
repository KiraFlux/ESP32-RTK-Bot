#include <Arduino.h>

#include "hardware/Motor.hpp"
#include "kf/Logger.hpp"
#include "kf/Storage.hpp"


static kf::Storage<Motor::Settings> left_motor_storage{
    .key = "lms",
    .settings = {.max_pwm = 255, .direction = Motor::Settings::Direction::CCW}
};

static kf::Storage<Motor::Settings> right_motor_storage{
    .key = "rms",
    .settings = {.max_pwm = 255, .direction = Motor::Settings::Direction::CW}
};

static const Motor left_motor{
    left_motor_storage.settings, GPIO_NUM_27,
    GPIO_NUM_21
};

static const Motor right_motor{
    right_motor_storage.settings, GPIO_NUM_19,
    GPIO_NUM_18
};

void setup() {
    Serial.begin(115200);

    kf_Logger_info("begin");

    kf::Logger::instance().write_func = [](const char *text, size_t length) { Serial.write(text, length); };

    left_motor.init();
    right_motor.init();

    left_motor_storage.load();
    right_motor_storage.load();

    kf_Logger_debug("init ok");
}

void loop() { delay(5000); }
