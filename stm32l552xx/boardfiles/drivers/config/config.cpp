#include "config.hpp"
#include <cstring>
#include <cstdio>
#include <string>

int Config::findParam(const char param[MAX_KEY_LENGTH], float &val) {
  char *key, *value;
  char msg[MAX_LINE_LENGTH];
  int length;
  
  while (fil.read(msg, sizeof(msg)) != nullptr) {
    key = strtok(msg, ",\n");
    value = strtok(NULL, ",\n");

    if (!strcmp(param, key)) break;
  }

  if (strcmp(param, key)) return 2;

  length = (fil.eof() != 0) ? strlen(value) : strlen(value) + 2; //accounting for \n

  fil.seek(fil.tell() - length);

  val = std::atof(value);

  return 0;
}

int Config::readParam(ConfigKey key, float &val) {
  if (fil.open() != 0) {
      return 1;
  }

  int res = findParam(config_table[static_cast<size_t>(key)].key, val);

  fil.close();

  if (res != 0) {
    return 2;
  }

  return 0;
}

int Config::writeParam(ConfigKey key, float newValue) {
  char strvalue[MAX_LINE_LENGTH];
  if (fil.open() != 0) {
    return 1;
  }

  float val;
  findParam(config_table[static_cast<size_t>(key)].key, val);

  snprintf(strvalue, MAX_LINE_LENGTH, "%g", newValue);

  if (strlen(strvalue) > MAX_VALUE_LENGTH) {
    strvalue[MAX_VALUE_LENGTH] = '\0';
  }

  fil.write(strvalue);

  if (strlen(strvalue) < MAX_VALUE_LENGTH) {
    for (int i = 0; i < 8 - static_cast<int>(strlen(strvalue)); i++) {
      fil.write(" ");
    }
  }

  fil.close();

  return 0;
}

#if defined(SWO_LOGGING)
extern "C" {
  int __io_putchar(int ch) {
    return ITM_SendChar(ch);
  }
}
#endif

int Config::init(ITextIO &fil) {
  this->fil = fil;
  
  if (this->fil.open() != 0) {
    return 1; // Error opening file
  }

  // Table was already initialized with default values so we loop and find all params in file
  for (size_t i = 0; i < static_cast<size_t>(ConfigKey::COUNT); i++) {
    // findParam sets the value in config_table[i].value with the value read from the file if it exists
    // If the parameter does not exist in the file, it will not change the value in config_table[i].value
    findParam(config_table[i].key, config_table[i].value);
  }

  // Close the file after reading all parameters
  if (this->fil.close() != 0) {
    return 2; // Error closing file
  }
}