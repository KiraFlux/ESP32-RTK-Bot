#pragma once

#include <kf/Text-UI.hpp>
#include <rs/aliases.hpp>

namespace zms {

struct HexDisplay final : kf::Widget {

private:
    kf::TextStream::Slice view;

public:
    explicit HexDisplay(kf::TextStream::Slice v) :
        view{std::move(v)} {}

    bool onEvent(kf::Event event) override {}

    void doRender(kf::TextStream &stream) const override {
        for (rs::size i = 0; i < view.len; i += 1) {
            const rs::u8 b = *(view.data + i);
            stream.print(b, HEX);
        }
    }
};

}// namespace zms
