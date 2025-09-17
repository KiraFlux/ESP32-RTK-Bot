#pragma once

#include <rs/aliases.hpp>

namespace zms {

// расстояние

/// Псевдоним типа выражения физической величины в миллиметрах (mm)
using Millimeters = rs::f64;

// Время

/// Выражает физическую величины отсчёта времении в секундах (s)
using Seconds = rs::f32;

/// Выражает физическую величины отсчёта времении в миллисекундах (ms)
using Milliseconds = rs::u32;

/// Выражает физическую величины отсчёта времении в микросекундах (us)
using Microseconds = rs::u32;

}// namespace zms
