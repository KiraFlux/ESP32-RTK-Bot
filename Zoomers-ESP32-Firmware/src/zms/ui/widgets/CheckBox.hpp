#pragma once

#include <functional>
#include <kf/Text-UI.hpp>
#include <rs/aliases.hpp>

namespace zms {

struct CheckBox final : kf::Widget {

    using Handler = std::function<void(bool)>;

private:
    Handler on_change;
    bool state;

public:
    explicit CheckBox(Handler change_handler, bool default_state = false) :
        on_change{std::move(change_handler)}, state{default_state} {}

    bool onEvent(kf::Event event) override {
        if (event == kf::Event::Click) {
            setState(not state);
            return true;
        }

        if (event == kf::Event::ChangeDecrement) {
            setState(false);
            return true;
        }

        if (event == kf::Event::ChangeIncrement) {
            setState(true);
            return true;
        }

        return false;
    }

    void doRender(kf::TextStream &stream) const override {
        stream.print(getLabel());
    }

private:
    inline rs::str getLabel() const { return state ? "[1]==" : "--[0]"; }

    void setState(bool new_state) {
        state = new_state;
        if (on_change) { on_change(state); }
    }
};

}// namespace zms
