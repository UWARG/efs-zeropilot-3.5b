// Managers that own config keys
enum class Owner : size_t {
    ATTITUDE_MANAGER = 0,
    COUNT,
};

// All config values and their default values and whether they require a reboot on change
#define CONFIG_KEYS_LIST(_) \
    _(BAUD_RATE,115200,false, Owner::ATTITUDE_MANAGER)