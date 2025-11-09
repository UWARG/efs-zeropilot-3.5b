// Managers that own config keys
enum class Owner : size_t {
    ATTITUDE_MANAGER = 0,
    TELEMETRY_MANAGER = 1,
    COUNT,
};

// All config values and their default values and whether they require a reboot on change, example values given
#define CONFIG_KEYS_LIST(_) \
    _(BAUD_RATE,115200,false, Owner::ATTITUDE_MANAGER) \
    _(GPS_ENABLED,true,false, Owner::TELEMETRY_MANAGER)
