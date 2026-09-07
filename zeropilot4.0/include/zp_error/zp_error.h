#pragma once
#include <cstdint>

enum [[nodiscard]] ZP_ERROR_e : uint32_t {
    ZP_ERROR_OK                   = 0,        // Operation completed successfully
    ZP_ERROR_FAIL                 = 1u << 0,  // Generic failure. Prioritize classifying errors listed below first.
    ZP_ERROR_UNSUPPORTED          = 1u << 1,  // Feature or operation not supported
    ZP_ERROR_CONFIG               = 1u << 2,  // Device or module is misconfigured
    ZP_ERROR_NULLPTR              = 1u << 3,  // Null pointer dereference attempt
    ZP_ERROR_INVALID_ARG          = 1u << 4,  // Provided input is malformed
    ZP_ERROR_RANGE                = 1u << 5,  // Input is well-formed but outside valid bounds
    ZP_ERROR_INVALID_DATA         = 1u << 6,  // Data is structurally valid but semantically wrong
    ZP_ERROR_PARSE                = 1u << 7,  // Failed to parse input
    ZP_ERROR_CRC                  = 1u << 8,  // CRC or verification check failed
    ZP_ERROR_NOT_READY            = 1u << 9,  // Resource is not initialized or ready
    ZP_ERROR_ALREADY_INITIALIZED  = 1u << 10, // Resource has already been initialized
    ZP_ERROR_BUSY                 = 1u << 11, // Resource or peripheral is busy
    ZP_ERROR_RESOURCE_UNAVAILABLE = 1u << 12, // Superset error for unavailable resource.
    ZP_ERROR_MEMORY_OVERFLOW      = 1u << 13, // Buffer or FIFO queue overflow
    ZP_ERROR_TIMEOUT              = 1u << 14, // Operation timed out
    ZP_ERROR_NACK                 = 1u << 15, // Peripheral did not acknowledge
    ZP_ERROR_EXT_API              = 1u << 16  // OR'd with a code above: failure came from HAL/RTOS/FatFs
};

// Deprecated alias, remove once every call site uses ZP_ERROR_INVALID_ARG
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
