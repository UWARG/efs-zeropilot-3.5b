#pragma once

#include <cstddef>

#include "config_keys_macro.hpp"

#define MAX_KEY_LENGTH 17
#define MAX_VALUE_LENGTH 10 // 1 for comma separator
#define MAX_LINE_LENGTH (MAX_KEY_LENGTH + MAX_VALUE_LENGTH + 2) // +2 for newline and separator (,)

enum class ConfigKey : size_t {
    #define _(name, default_val, reboot_flag, owner) name,
        CONFIG_KEYS_LIST(_)
    #undef _
    COUNT
};

typedef struct {
    char key[MAX_KEY_LENGTH];
    float value;
    bool reboot_on_change = false;
    Owner owner;
} Param_t;

static constexpr size_t NUM_KEYS = static_cast<size_t>(ConfigKey::COUNT);
