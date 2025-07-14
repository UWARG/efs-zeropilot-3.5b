#pragma once

#include "config_iface.hpp"
#include "textio_iface.hpp"

#define MAX_LINE_LENGTH 128
#define MAX_VALUE_LENGTH 32
#define MAX_KEY_LENGTH (MAX_LINE_LENGTH-MAX_VALUE_LENGTH)

struct KeyValue {
    const char key[MAX_KEY_LENGTH];
    float value;
    bool reboot_on_change = false;
}

class Config : public IConfig {
    private:
        ITextIO fil;

        KeyValue config_table[NUM_KEYS] = {
            #define _(name, default_val, reboot_flag) {#name, default_val, reboot_flag},
                CONFIG_KEYS_LIST(_)
            #undef _
        };

        /**
         * @brief Moves read/write pointer to parameter
         * @param param parameter to find
         * @param val buffer to store value at parameter
         * @retval Operation success
         */
        int findParam(const char param[100], float &val);

    public:
        Config(TextIO &fil) = default;

        /**
         * @brief reads parameter from config file
         * @param param parameter to read
         * @param val buffer to store value
         * @retval Operation success
         */
        int readParam(ConfigKey key, float &val) override;

        /**
         * @brief writes parameter from config file
         * @param param parameter to write too
         * @param newValue updated value
         * @retval Operation success
         */
        int writeParam(ConfigKey key, float newValue) override;

        /**
         * @brief Adds textio object and reads config file
         * @param fil TextIO object to read config file
         * @retval Operation success
         */
        int init(ITextIO &fil);
};