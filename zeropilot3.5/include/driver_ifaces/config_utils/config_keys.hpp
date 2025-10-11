#pragma once

#include <cstddef>

#include "config_keys.inc"

enum class ConfigKey : size_t {
    #define _(name, default_val, reboot_flag, owner) name,
        CONFIG_KEYS_LIST(_)
    #undef _
    COUNT
};

static constexpr size_t NUM_KEYS = static_cast<size_t>(ConfigKey::COUNT);
