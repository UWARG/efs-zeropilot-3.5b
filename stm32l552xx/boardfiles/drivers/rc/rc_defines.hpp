#pragma once
#include <cstdint>

/* define range mapping here, -+100% -> 1000..2000 */
static constexpr uint8_t SBUS_RANGE_MIN = 192;
static constexpr uint16_t SBUS_RANGE_MAX = 1792;
static constexpr uint16_t SBUS_RANGE_RANGE = (SBUS_RANGE_MAX - SBUS_RANGE_MIN);

static constexpr uint8_t HEADER_ = 0x0F;
static constexpr uint8_t FOOTER_ = 0x00;
static constexpr uint8_t FOOTER2_ = 0x04;
static constexpr uint8_t CH17_MASK_ = 0x01;
static constexpr uint8_t CH18_MASK_ = 0x02;
static constexpr uint8_t LOST_FRAME_MASK_ = 0x04;
static constexpr uint8_t FAILSAFE_MASK_ = 0x08;

