#include "config.hpp"
#include <cstring>
#include <cstdio>
#include <string>

Config::Config(ITextIO* textIO) : textIO(textIO) {
  //blank
}

int Config::init() {

  snprintf(configFile, 100, "config.txt");
  if (textIO->open(configFile) != 0) {
    return 1; // Error opening file
  }

  // Table was already initialized with default values so we loop and find all params in file
  for (size_t i = 0; i < static_cast<size_t>(ConfigKey::COUNT); i++) {
    // findParam sets the value in config_table[i].value with the value read from the file if it exists
    // If the parameter does not exist in the file, it will not change the value in config_table[i].value
    findParam(config_table[i].key, config_table[i].value);
  }

  // Close the file after reading all parameters
  if (textIO->close() != 0) {
    return 2; // Error closing file
  }
}

int Config::findParam(const char param[MAX_KEY_LENGTH], float &val, int &tableIdx) {
  // move read/write pointer to param value
  char *key, *value;
  char msg[MAX_LINE_LENGTH];
  int length;
  
  while (textIO->read(msg, sizeof(msg)) != nullptr) {
    key = strtok(msg, ",\n");
    value = strtok(NULL, ",\n");

    if (!strcmp(param, key)) break;
  }

  if (strcmp(param, key)) return 2;

  length = (textIO->eof() != 0) ? strlen(value) : strlen(value) + 2; //accounting for \n

  textIO->seek(textIO->tell() - length);

  val = std::atof(value);

  // find idx of param in config table
  int idx = 0;
  while (strcmp(config_table[idx].key, param)) {
    idx++;
  }

  tableIdx = idx;

  return 0;
}

int Config::findParam(const char param[100], float &val)  {
  int tblIdx;
  return findParam(param, val, tblIdx);
}

int Config::findParam(const char param[100], int &tableIdx)  {
  float val;
  return findParam(param, val, tableIdx);
}

int Config::readParam(ConfigKey key, float &val) {
  if (textIO->open(configFile) != 0) {
      return 1;
  }
  
  int res = findParam(config_table[static_cast<size_t>(key)].key, val);

  textIO->close();

  if (res != 0) {
    return 2;
  }

  return 0;
}

int Config::writeParam(ConfigKey key, float newValue) {
  char strvalue[MAX_LINE_LENGTH];
  if (textIO->open(configFile) != 0) {
    return 1;
  }

  float val;
  int tblIdx;
  findParam(config_table[static_cast<size_t>(key)].key, val, tblIdx);

  snprintf(strvalue, MAX_LINE_LENGTH, "%g", newValue);

  if (strlen(strvalue) > MAX_VALUE_LENGTH) {
    strvalue[MAX_VALUE_LENGTH] = '\0';
  }

  int writeRes = textIO->write(strvalue);

  if (strlen(strvalue) < MAX_VALUE_LENGTH) {
    for (int i = 0; i < 8 - static_cast<int>(strlen(strvalue)); i++) {
      textIO->write(" ");
    }
  }

  textIO->close();

  // on success, write to config table
  if (writeRes > 0) {
    config_table[tblIdx].value = newValue;
  }
  
  return 0;
}

Owner Config::getParamOwner(ConfigKey key) {
  size_t index = static_cast<size_t>(key);
  if (index >= NUM_KEYS) {
      return Owner::COUNT; // Invalid owner
  }
  return config_table[index].owner;
}

#if defined(SWO_LOGGING)
extern "C" {
  int __io_putchar(int ch) {
    return ITM_SendChar(ch);
  }
}
#endif