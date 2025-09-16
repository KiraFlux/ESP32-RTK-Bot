#pragma once

/// @brief Низкочастотный фильтры
/// @tparam T Скаляр
template<typename T> struct LowFrequencyFilter {

private:
    const float alpha;
    const float one_minus_alpha{1.0f - alpha};
    T filtered{};
    bool first_step{false};

public:
    explicit LowFrequencyFilter(float alpha) noexcept :
        alpha{alpha} {}

    const T &calc(const T &x) noexcept {
        if (first_step) {
            first_step = false;
            filtered = x;
            return filtered;
        }

        if (alpha == 1.0) {
            filtered = x;
            return filtered;
        }

        filtered = filtered * one_minus_alpha + x * alpha;
        return filtered;
    }

    /// Сбросить значение фильтра
    void reset() {
        first_step = true;
    }
};
