#pragma once

#include <kf/Text-UI.hpp>

namespace zms {

template<typename T> struct Display final : kf::Widget {
    const T &value;

    explicit Display(const T &val) :
        value{val} {}

    bool onEvent(kf::Event event) override { return false; }

    void doRender(kf::TextStream &stream) const override { stream.print(value); }
};

}// namespace zms
