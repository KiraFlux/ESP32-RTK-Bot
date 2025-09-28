#pragma once

#include <zms/aliases.hpp>

namespace zms {

/// Задачи
struct Task {

    /// Состояние задачи
    enum State : rs::u8 {

        /// Бездействие
        Idle = 0x00,

        /// Прямолинейное движение на расстояние
        GoDist = 0x01,

        /// Поворот на месте
        Turn = 0x02,

        ///
        Aligh = 0x03,
    
        ///
        Hardcode = 0x04
    };

    /// arg
    rs::f32 arg{0.0f};

    /// speed
    rs::f32 speed{0.3f};

    /// result
    rs::u32 result{0};

    /// Текущее состояние
    State current_state{State::Idle};
};

}// namespace zms
