#pragma once

#include <cstdint>

static constexpr uint8_t SBUS_BYTE_COUNT = 25;
static constexpr uint8_t SBUS_CHANNEL_COUNT = 16;
static constexpr uint8_t SBUS_MAX_BTYES_PER_CHANNEL = 3;

/* define range mapping here, -+100% -> 1000..2000 */
static constexpr uint16_t SBUS_RANGE_MIN = 192;
static constexpr uint16_t SBUS_RANGE_MAX = 1792;
static constexpr uint16_t SBUS_RANGE_RANGE = (SBUS_RANGE_MAX - SBUS_RANGE_MIN);

static constexpr uint8_t HEADER_ = 0x0F;
static constexpr uint8_t FOOTER_ = 0x00;
