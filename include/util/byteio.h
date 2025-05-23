#pragma once

#include <vector>
#include <cstdint>

// Appends a uint32_t to the buffer in little-endian byte order
inline void write_u32(std::vector<uint8_t>& buf, uint32_t value) {
    buf.push_back((value >> 0) & 0xFF);
    buf.push_back((value >> 8) & 0xFF);
    buf.push_back((value >> 16) & 0xFF);
    buf.push_back((value >> 24) & 0xFF);
}
