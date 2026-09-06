#pragma once
#include <cstdint>

// ZP_ERROR_e is a bitmask, not an ordinal. Errors accumulate with |= so that a
// sequence of operations reports every failure it hit, not just the first.
//
// Two rules follow from ZP_ERROR_OK being 0:
//
//   1. Never test a ZP_ERROR_e for truthiness. `if (!doThing())` reads as "if it
//      failed" but means "if it SUCCEEDED". Always compare explicitly against
//      ZP_ERROR_OK.
//   2. Never discard a returned ZP_ERROR_e. The type is [[nodiscard]], so the
//      compiler enforces this; silence a deliberate discard with a (void) cast
//      and a comment saying why.
//
// This is a C++-only header: it uses <cstdint> and operator overloads. Do not
// include it from a .c translation unit.

enum [[nodiscard]] ZP_ERROR_e : uint32_t {
    ZP_ERROR_OK                   = 0,

    // --- General ---
    ZP_ERROR_FAIL                 = 1u << 0,   // Generic failure. Prefer a specific code below.
    ZP_ERROR_UNSUPPORTED          = 1u << 1,   // Feature or operation not supported
    ZP_ERROR_CONFIG               = 1u << 2,   // Device or module is misconfigured

    // --- Argument and data validation ---
    ZP_ERROR_NULLPTR              = 1u << 3,   // Null pointer argument
    ZP_ERROR_INVALID_ARG          = 1u << 4,   // Argument is malformed or nonsensical
    ZP_ERROR_RANGE                = 1u << 5,   // Argument is well-formed but outside valid bounds
    ZP_ERROR_INVALID_DATA         = 1u << 6,   // Data is structurally valid but semantically wrong

    // --- Integrity ---
    ZP_ERROR_PARSE                = 1u << 7,   // Failed to parse input
    ZP_ERROR_CRC                  = 1u << 8,   // Checksum or verification mismatch

    // --- Lifecycle ---
    ZP_ERROR_NOT_READY            = 1u << 9,   // Resource is not initialized or ready
    ZP_ERROR_ALREADY_INITIALIZED  = 1u << 10,  // Resource has already been initialized
    ZP_ERROR_BUSY                 = 1u << 11,  // Resource or peripheral is busy

    // --- Resources ---
    ZP_ERROR_RESOURCE_UNAVAILABLE = 1u << 12,  // Superset error for unavailable resource
    ZP_ERROR_MEMORY_OVERFLOW      = 1u << 13,  // Buffer or FIFO queue overflow

    // --- Comms ---
    ZP_ERROR_TIMEOUT              = 1u << 14,  // Operation timed out
    ZP_ERROR_NACK                 = 1u << 15,  // Peripheral did not acknowledge

    // --- Origin ---
    ZP_ERROR_EXT_API              = 1u << 16,  // OR'd with a code above: the failure came from
                                               // third-party code (ST HAL, CMSIS-RTOS, FatFs, ...)
                                               // rather than from our own logic.

    // bits 17-31 free
};

// Deprecated: renamed to ZP_ERROR_INVALID_ARG. Kept so the tree builds through
// the main merge; remove once every call site is updated.
constexpr ZP_ERROR_e ZP_ERROR_INVALID_PARAM = ZP_ERROR_INVALID_ARG;

// Overload for bitwise accumulation
inline ZP_ERROR_e& operator|=(ZP_ERROR_e& lhs, ZP_ERROR_e rhs) {
    lhs = static_cast<ZP_ERROR_e>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    return lhs;
}

inline constexpr ZP_ERROR_e operator|(ZP_ERROR_e lhs, ZP_ERROR_e rhs) {
    return static_cast<ZP_ERROR_e>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline constexpr ZP_ERROR_e operator&(ZP_ERROR_e lhs, ZP_ERROR_e rhs) {
    return static_cast<ZP_ERROR_e>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}
