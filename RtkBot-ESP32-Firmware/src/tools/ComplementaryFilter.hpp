#pragma once

template<typename T> struct ComplementaryFilter {

private:
    const float alpha;
    const float one_minus_alpha{1.0f - alpha};
    T filtered{};
    bool first_step{true};

public:
    explicit ComplementaryFilter(float alpha) :
        alpha{alpha} {}

    const T &calc(T x, T dx, float dt) {
        if (first_step) {
            first_step = false;
            filtered = x;
        } else {
            T prediction = filtered + dx * dt;
            filtered = alpha * prediction + one_minus_alpha * x;
        }

        return filtered;
    }

    void reset() {
        first_step = true;
    }
};