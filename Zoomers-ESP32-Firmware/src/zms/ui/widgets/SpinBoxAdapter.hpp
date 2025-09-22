#pragma once

#include <kf/tui.hpp>

namespace zms {
// fixme
template<typename T> struct SpinBoxAdapter final : kf::Widget {
    using Input = kf::SpinBox<T>;
    using Mode = typename Input::Mode;
    using Scalar = typename Input::Scalar;

private:

private:
    T step;
    const T step_step;
    Input value_input;
    Input step_input;
    bool value_input_mode{true};

public:
    explicit SpinBoxAdapter(
        T &value_param,
        Mode value_input_mode_param,
        T value_step_param,
        T value_step_step_param,
        Mode step_input_mode_param
    ) :
        step(value_step_param),
        step_step(value_step_step_param),
        value_input(value_param, this->step, value_input_mode_param),
        step_input(this->step, this->step_step, step_input_mode_param) {}

    bool onEvent(kf::Event event) override {
        if (event == kf::Event::Click) {
            value_input_mode ^= 1;
            return true;
        }

        if (value_input_mode) {
            return value_input.onEvent(event);
        } else {
            return step_input.onEvent(event);
        }
    }

    void doRender(kf::TextStream &stream) const override {
        if (value_input_mode) {
            value_input.doRender(stream);
        } else {
            stream.write('s');
            step_input.doRender(stream);
        }
    }
};

}