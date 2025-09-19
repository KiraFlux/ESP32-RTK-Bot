#pragma once

#include <array>

#include <kf/Text-UI.hpp>
#include <rs/aliases.hpp>


namespace zms {

template<typename T, rs::size N> struct ComboBox final : kf::Widget {
    static_assert(N >= 1, "N >= 1");

public:

    struct Item {
        rs::str key;
        T value;
    };

    using Container = std::array<Item, N>;

private:

    const Container items;
    T &value;
    int cursor{0};

public:

    explicit ComboBox(Container items, T &val) :
        items{std::move(items)}, value{val} {}

    bool onEvent(kf::Event event) override {
        if (event == kf::Event::ChangeDecrement) {
            moveCursor(-1);
            value = items[cursor].value;
            return true;
        }
        if (event == kf::Event::ChangeIncrement) {
            moveCursor(+1);
            value = items[cursor].value;
            return true;
        }
        return false;
    }

    void doRender(kf::TextStream &stream) const override {
        stream.write('<');
        stream.print(items[cursor].key);
        stream.write('>');
    }

private:

    void moveCursor(int d) {
        cursor += d;
        cursor += N;
        cursor %= N;
    }

};

}