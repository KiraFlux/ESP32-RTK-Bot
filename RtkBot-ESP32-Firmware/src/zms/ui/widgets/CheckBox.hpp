#pragma once

#include <array>
#include <utility>

#include <kf/Text-UI.hpp>
#include <rs/aliases.hpp>


namespace zms {

template<typename T, rs::size N> struct CheckBox final : kf::Widget {
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

    explicit CheckBox(Container items, T &val) :
        items{std::move(items)}, value{val} {}

    bool onEvent(kf::Event event) override {
        if (event == kf::Event::ChangeDecrement and moveCursor(-1)) {
            value = items[cursor].value;
            return true;
        }
        if (event == kf::Event::ChangeIncrement and moveCursor(+1)) {
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

    bool moveCursor(int d) {
        const auto old_cursor = cursor;
        cursor += d;
        cursor = (constrain(cursor, 0, N - 1));
        return old_cursor != cursor;
    }

};

}