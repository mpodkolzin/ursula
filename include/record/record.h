#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include "util/crc32.h"

enum class RecordType : uint8_t {
    DATA = 1,
    SNAPSHOT = 2,
    HEARTBEAT = 3,
};

class Record {
public:
    static constexpr uint32_t MAGIC = 0xFACADE01;

    Record(RecordType type, std::vector<uint8_t> payload);

    std::vector<uint8_t> serialize() const;
    static Record deserialize(const std::vector<uint8_t>& buffer, size_t& offset);

    RecordType type() const;
    const std::vector<uint8_t>& payload() const;

private:
    RecordType type_;
    std::vector<uint8_t> payload_;

    static uint32_t calculate_crc(const std::vector<uint8_t>& data);
    static uint32_t read_u32(const std::vector<uint8_t>& buf, size_t& offset);
    static uint8_t read_u8(const std::vector<uint8_t>& buf, size_t& offset);
    static void write_u32(std::vector<uint8_t>& buf, uint32_t value);
    static void write_u8(std::vector<uint8_t>& buf, uint8_t value);
};
