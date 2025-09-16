#pragma once

#include <kf/Text-UI.hpp>

template<typename W> struct EventObserver : kf::Widget {
    static_assert((std::is_base_of<kf::Widget, W>::value), "W must be a kf::Widget Subclass");

private:
    std::function<void(kf::Event)> handler;
    W w;

public:
    using Observable = W;

    explicit EventObserver(W w, std::function<void(kf::Event)> _handler) :
        w{std::move(w)}, handler{std::move(_handler)} {}

    bool onEvent(kf::Event event) override {
        const auto ret = w.onEvent(event);
        if (handler) { handler(event); }
        return ret;
    }

    void doRender(kf::TextStream &stream) const override {
        w.doRender(stream);
    }
};