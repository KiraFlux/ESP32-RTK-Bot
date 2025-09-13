#pragma once

#include <cstdint>
#include <array>
#include <Print.h>
#include <utility>
#include <vector>
#include <queue>
#include <functional>

#include "tools/Singleton.hpp"

/// Text User Interface
namespace tui {

enum class Event {

    None,

    /// Принудительное обновление
    Update,

    /// Клик
    Click,

    /// Выбор следующего элемента
    ElementNext,

    /// Смена элемента
    ElementPrevious,

    /// Изменить элемент +
    ChangeIncrement,

    /// Изменить элемент -
    ChangeDecrement,
};

struct TextStream final : Print {
    static constexpr size_t buffer_size = 128;

private:

    std::array<char, buffer_size> buffer{};
    size_t cursor{0};

public:

    struct Slice {
        const char *data;
        const size_t len;
    };

    Slice prepareData() {
        buffer[cursor] = '\0';

        return {
            buffer.data(),
            cursor,
        };
    }

    void reset() { cursor = 0; }

    size_t write(uint8_t c) override {
        if (cursor < buffer_size) {
            buffer[cursor] = static_cast<char>(c);
            cursor += 1;
            return 1;
        }
        return 0;
    }
};

struct Widget {
    virtual bool onEvent(Event event) = 0;

    virtual void doRender(TextStream &stream) const = 0;

    void render(TextStream &stream, bool selected) const {
        if (selected) { stream.write(0x81); }
        doRender(stream);
        if (selected) { stream.write(0x80); }
    }
};

struct Button final : Widget {

    using ClickHandler = std::function<void(Button &)>;

    const char *label;
    ClickHandler on_click;

    explicit Button(const char *label, ClickHandler on_click = nullptr) noexcept:
        label{label}, on_click{std::move(on_click)} {}

    bool onEvent(Event event) override {
        if (event == Event::Click and on_click) {
            on_click(*this);
        }
        return false;
    }

    void doRender(TextStream &stream) const override {
        stream.write('[');
        stream.print(label);
        stream.write(']');
    }
};

template<typename T> struct Display final : Widget {
    const T &value;

    bool onEvent(Event event) override { return false; }

    void doRender(TextStream &stream) const override { stream.print(value); }
};

template<typename T> struct SpinBox final : Widget {
    static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");

    using Scalar = T;

    enum class Mode {
        Arithmetic,
        ArithmeticPositiveOnly,
        Geometric
    };

    T &value;
    const T &step;
    const Mode mode;

    explicit SpinBox(T &value, const T &step, Mode mode = Mode::Arithmetic) noexcept:
        value{value}, step{step}, mode{mode} {}

    bool onEvent(Event event) override {
        if (event == Event::ChangeIncrement) {
            if (mode == Mode::Geometric) {
                value *= step;
            } else {
                value += step;
            }
            return true;
        }

        if (event == Event::ChangeDecrement) {
            if (mode == Mode::Geometric) {
                value /= step;
            } else {
                value -= step;
                if (mode == Mode::ArithmeticPositiveOnly && value < 0) {
                    value = 0;
                }
            }
            return true;
        }

        return false;
    }

    void doRender(TextStream &stream) const override {
        stream.write('<');

        if (std::is_floating_point<T>::value) {
            stream.print(static_cast<float>(value), 4);
        } else {
            stream.print(value);
        }
        stream.write('>');
    }
};

template<typename W> struct Labeled final : Widget {
    static_assert((std::is_base_of<Widget, W>::value), "W must be a Widget Subclass");

    using Content = W;

    const char *label;
    W content;

    explicit Labeled(const char *label, W content) noexcept:
        label{label}, content{std::move(content)} {}

    bool onEvent(Event event) override {
        return content.onEvent(event);
    }

    void doRender(TextStream &stream) const override {
        stream.print(label);
        stream.write(0x82);
        stream.write(':');
        content.doRender(stream);
    }

};

struct Page;

struct PageSetterButton final : Widget {
    Page &target;

    explicit PageSetterButton(Page &target) :
        target{target} {}

    bool onEvent(Event event) override;

    void doRender(TextStream &stream) const override;
};

struct Page {

    const char *title;

private:

    std::vector<Widget *> widgets{};
    int cursor{0};
    PageSetterButton to_this{*this};

public:

    explicit Page(const char *title) noexcept:
        title{title} {}

    void add(Widget &widget) { widgets.push_back(&widget); }

    void link(Page &other) {
        this->add(other.to_this);
        other.add(this->to_this);
    }

    void render(TextStream &stream, int rows) {
        stream.print(title);
        stream.write('\n');

        rows -= 1;

        const int start = (totalWidgets() > rows) ? std::min(cursor, totalWidgets() - rows) : 0;
        const int end = std::min(start + rows, totalWidgets());

        for (int i = start; i < end; i++) {
            widgets[i]->render(stream, i == cursor);
            stream.write('\n');
        }
    }

    bool onEvent(Event event) {
        switch (event) {
            case Event::None:
                return false;

            case Event::Update:
                return true;

            case Event::ElementNext:
                cursorMove(+1);
                return true;

            case Event::ElementPrevious:
                cursorMove(-1);
                return true;

            case Event::Click:
            case Event::ChangeIncrement:
            case Event::ChangeDecrement:
                return widgets[cursor]->onEvent(event);
        }

        return false;
    }

private:

    void cursorMove(int delta) {
        cursor += delta;
        cursor = std::max(cursor, 0);
        cursor = std::min(cursor, cursorPositionMax());
    }

    inline int totalWidgets() const { return static_cast<int>(widgets.size()); }

    inline int cursorPositionMax() const { return totalWidgets() - 1; }
};

struct PageManager final : Singleton<PageManager> {
    friend struct Singleton<PageManager>;

private:

    std::queue<Event> events{};
    TextStream stream{};
    Page *active_page{nullptr};
    Page *previous_page{nullptr};

public:

    int rows{8};

    void bind(Page &page) {
        previous_page = active_page;
        active_page = &page;
    }

    void back() {
        std::swap(previous_page, active_page);
    }

    TextStream::Slice render() {
        static constexpr char null_page_content[] = "null page";
        static constexpr TextStream::Slice null_page_slice{null_page_content, sizeof(null_page_content)};

        if (active_page == nullptr) {
            return null_page_slice;
        }

        stream.reset();
        active_page->render(stream, rows);

        return stream.prepareData();
    }

    void addEvent(Event event) {
        events.push(event);
    }

    bool pollEvents() {
        if (active_page == nullptr) {
            return false;
        }

        if (events.empty()) {
            return false;
        }

        const bool render_required = active_page->onEvent(events.front());
        events.pop();
        return render_required;
    }
};

void PageSetterButton::doRender(TextStream &stream) const {
    stream.write('>');
    stream.write(' ');
    stream.print(target.title);
}

bool PageSetterButton::onEvent(Event event) {
    if (event == Event::Click) {
        PageManager::instance().bind(target);
        return true;
    }
    return false;
}

}