#pragma once

#include <Preferences.h>

#include "Logger.hpp"


/// Настройки
template<typename T> struct Storage final {

private:

    static constexpr const char *preferences_namespace = "RTK-cfg";

public:

    const char *key;
    T settings;

    bool load() {
        Logger_debug("Loading storage %s", key);

        Preferences preferences;

        if (not begin(preferences, true)) {
            return false;
        }

        const auto size = preferences.getBytesLength(key);

        if (size != sizeof(T)) {
            preferences.end();
            Logger_error("%s read fail", key);
            return false;
        }

        preferences.getBytes(key, &settings, sizeof(T));
        preferences.end();

        return true;
    }

    /// Записывает в FLASH
    bool save() {
        Logger_debug("Saving storage %s", key);

        Preferences preferences;
        if (not begin(preferences, false)) {
            return false;
        }

        const auto saved = preferences.putBytes(key, &settings, sizeof(T));
        preferences.end();

        return saved == sizeof(T);
    }

    /// Стирает данные хранилища во FLASH
    bool erase() {
        Logger_debug("Saving storage %s", key);

        Preferences preferences;
        if (not begin(preferences, false)) {
            return false;
        }

        if (not preferences.remove(key)) {
            preferences.end();
            Logger_error("key %s remove fail", key);
            return false;
        }

        return true;
    }

private:

    bool begin(Preferences &preferences, bool read_only) const {
        if (preferences.begin(preferences_namespace, read_only)) {
            return true;
        } else {
            Logger_error("%s begin fail", key);
            return false;
        }
    }

public:

    Storage(const Storage &) = delete;

};