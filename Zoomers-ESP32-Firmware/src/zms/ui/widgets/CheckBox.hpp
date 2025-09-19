#pragma once

#include <functional>
#include <kf/Text-UI.hpp>
#include <rs/aliases.hpp>

namespace zms {

struct CheckBox final : kf::Widget {

    using Handler = std::function<void(bool)>;

private:
    Handler on_change;
    bool state{false};

public:
    explicit CheckBox(Handler change_handler) :
        on_change{std::move(change_handler)} {}

    bool onEvent(kf::Event event) override {
        if (event == kf::Event::Click) {
            state = not state;
            return true;
        }

        if (event == kf::Event::ChangeDecrement) {
            state = false;
            return true;
        }

        if (event == kf::Event::ChangeIncrement) {
            state = true;
            return true;
        }

        return false;
    }

    void doRender(kf::TextStream &stream) const override {
        stream.print(getLabel());
    }

private:
    rs::str getLabel() const {
        return state ? "==[X]" : "[ ]--";
    }
};

}// namespace zms
