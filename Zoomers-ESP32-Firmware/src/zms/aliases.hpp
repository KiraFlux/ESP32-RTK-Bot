#pragma once

#include <rs/aliases.hpp>

namespace zms {

// расстояние

/// @brief Псевдоним типа выражения физической величины в миллиметрах (mm)
using Millimeters = rs::f64;

// Время

/// @brief Выражает физическую величины отсчёта времени в секундах (s)
using Seconds = rs::f32;

/// @brief Выражает физическую величины отсчёта времени в миллисекундах (ms)
using Milliseconds = rs::u32;

/// @brief Выражает физическую величины отсчёта времени в микросекундах (us)
using Microseconds = rs::u32;

}// namespace zms
