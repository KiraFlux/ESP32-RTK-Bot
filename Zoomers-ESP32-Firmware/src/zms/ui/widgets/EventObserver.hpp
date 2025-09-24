#pragma once

#include <kf/tui.hpp>

namespace zms {

template<typename W> struct EventObserver : kf::tui::Widget {
    static_assert((std::is_base_of<Widget, W>::value), "W must be a kf::Widget Subclass");

private:
    std::function<void(kf::tui::Event)> handler;
    W w;

public:
    using Observable = W;

    explicit EventObserver(W w, std::function<void(kf::tui::Event)> _handler) :
        w{std::move(w)}, handler{std::move(_handler)} {}

    bool onEvent(kf::tui::Event event) override {
        const auto ret = w.onEvent(event);
        if (handler) { handler(event); }
        return ret;
    }

    void doRender(kf::tui::TextStream &stream) const override {
        w.doRender(stream);
    }
};

}// namespace zms
