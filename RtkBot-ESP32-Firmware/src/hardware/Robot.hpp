#pragma once

#include "tools/Singleton.hpp"

#include "hardware/Motor.hpp"

struct Robot : Singleton<Robot> {
  friend struct Singleton<Robot>;
}