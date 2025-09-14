#pragma once

#include "config_keys.hpp"
#include "textio_iface.hpp"

#define MAX_LINE_LENGTH 128
#define MAX_VALUE_LENGTH 32
#define MAX_KEY_LENGTH (MAX_LINE_LENGTH-MAX_VALUE_LENGTH)

typedef struct {
    const char key[MAX_KEY_LENGTH];
    float value;
    bool reboot_on_change = false;
} Param_t;

class Config {
    private:
        char configFile[100];
        ITextIO *textIO;

        Param_t config_table[NUM_KEYS] = {
            #define _(name, default_val, reboot_flag) {#name, default_val, reboot_flag},
                CONFIG_KEYS_LIST(_)
            #undef _
        };

        /**
         * @brief Moves read/write pointer to parameter
         * @param param parameter to find
         * @param val buffer to store value at parameter
         * @param tableIdx buffer to store config table index of parameter
         * @retval Operation success
         */
        int findParam(const char param[100], float &val, int &tableIdx);
        int findParam(const char param[100], float &val);
        int findParam(const char param[100], int &tableIdx);

    public:
        Config(ITextIO *textIO);

        /**
         * @brief Adds textIO object and reads config file
         * @retval Operation success
         */
        int init();

        /**
         * @brief reads parameter from config file
         * @param param parameter to read
         * @param val buffer to store value
         * @retval Operation success
         */
        int readParam(ConfigKey key, float &val);

        /**
         * @brief writes parameter from config file
         * @param param parameter to write too
         * @param newValue updated value
         * @retval Operation success
         */
        int writeParam(ConfigKey key, float newValue);
};