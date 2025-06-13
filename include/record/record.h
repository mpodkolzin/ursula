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
    Record(RecordType type, std::string& payload);

    std::vector<uint8_t> serialize() const;
    static Record deserialize(const std::vector<uint8_t>& buffer, size_t& offset);

    RecordType type() const;
    const std::vector<uint8_t>& payload() const;

    inline void serialize_into(std::vector<uint8_t>& out) const {
        uint32_t payload_len = static_cast<uint32_t>(payload_.size());
        uint32_t crc = calculate_crc(payload_);

        // Reserve in advance to reduce reallocation
        out.reserve(out.size() + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t) + payload_len + sizeof(uint32_t));

        write_u32(out, MAGIC);
        write_u8(out, static_cast<uint8_t>(type_));
        write_u32(out, payload_len);
        out.insert(out.end(), payload_.begin(), payload_.end());
        write_u32(out, crc);
    }

private:
    RecordType type_;
    std::vector<uint8_t> payload_;

    static uint32_t calculate_crc(const std::vector<uint8_t>& data);
    static uint32_t read_u32(const std::vector<uint8_t>& buf, size_t& offset);
    static uint8_t read_u8(const std::vector<uint8_t>& buf, size_t& offset);
    static void write_u32(std::vector<uint8_t>& buf, uint32_t value);
    static void write_u8(std::vector<uint8_t>& buf, uint8_t value);
};
