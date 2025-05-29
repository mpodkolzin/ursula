#include "record/record.h"
#include <spdlog/spdlog.h>

Record::Record(RecordType type, std::vector<uint8_t> payload)
    : type_(type), payload_(std::move(payload)) {}

Record::Record(RecordType type, std::string& payload)
    : type_(type)
    {
        payload_.insert(payload_.end(), payload.begin(), payload.end());
    }

RecordType Record::type() const {
    return type_;
}

const std::vector<uint8_t>& Record::payload() const {
    return payload_;
}

std::vector<uint8_t> Record::serialize() const {
    std::vector<uint8_t> result;

    // Write MAGIC
    write_u32(result, MAGIC);

    // Length = payload size
    write_u32(result, static_cast<uint32_t>(payload_.size()));

    // Prepare crc input: [TYPE][PAYLOAD]
    std::vector<uint8_t> crc_input;
    write_u8(crc_input, static_cast<uint8_t>(type_));
    crc_input.insert(crc_input.end(), payload_.begin(), payload_.end());

    // Write CRC
    write_u32(result, calculate_crc(crc_input));

    // Write TYPE
    write_u8(result, static_cast<uint8_t>(type_));

    // Write PAYLOAD
    result.insert(result.end(), payload_.begin(), payload_.end());

    return result;
}

Record Record::deserialize(const std::vector<uint8_t>& buffer, size_t& offset) {
    spdlog::debug("Record::deserialize: offset='{}'", offset);
    uint32_t magic = read_u32(buffer, offset);
    if (magic != MAGIC) {
        spdlog::error("Record::deserialize: Invalid record magic number='{}'", magic);
        throw std::runtime_error("Invalid record magic number");
    }

    uint32_t length = read_u32(buffer, offset);
    uint32_t expected_crc = read_u32(buffer, offset);
    RecordType type = static_cast<RecordType>(read_u8(buffer, offset));

    if (offset + length > buffer.size()) {
        spdlog::error("Record::deserialize: Payload length exceeds buffer size");
        throw std::runtime_error("Payload length exceeds buffer size");
    }

    std::vector<uint8_t> payload(buffer.begin() + offset, buffer.begin() + offset + length);

    // Verify CRC
    std::vector<uint8_t> crc_input;
    write_u8(crc_input, static_cast<uint8_t>(type));
    crc_input.insert(crc_input.end(), payload.begin(), payload.end());
    uint32_t actual_crc = calculate_crc(crc_input);

    if (actual_crc != expected_crc) {
        spdlog::error("Record::deserialize: CRC mismatch in record");
        throw std::runtime_error("CRC mismatch in record");
    }

    offset += length;
    return Record(type, std::move(payload));
}

uint32_t Record::calculate_crc(const std::vector<uint8_t>& data) {
    // Placeholder — replace with a proper CRC32 implementation
    uint32_t crc = 0;
    for (auto b : data) {
        crc += b;
    }
    spdlog::debug("Record::calculate_crc: crc='{}'", crc);
    return crc; // NOT safe — just for compileability if no CRC lib linked
}

uint32_t Record::read_u32(const std::vector<uint8_t>& buf, size_t& offset) {
    if (offset + 4 > buf.size()) throw std::runtime_error("read_u32 out of bounds");
    uint32_t value = 0;
    value |= static_cast<uint32_t>(buf[offset++]) << 24;
    value |= static_cast<uint32_t>(buf[offset++]) << 16;
    value |= static_cast<uint32_t>(buf[offset++]) << 8;
    value |= static_cast<uint32_t>(buf[offset++]);
    return value;
}

uint8_t Record::read_u8(const std::vector<uint8_t>& buf, size_t& offset) {
    if (offset >= buf.size()) throw std::runtime_error("read_u8 out of bounds");
    return buf[offset++];
}

void Record::write_u32(std::vector<uint8_t>& buf, uint32_t value) {
    buf.push_back((value >> 24) & 0xFF);
    buf.push_back((value >> 16) & 0xFF);
    buf.push_back((value >> 8) & 0xFF);
    buf.push_back(value & 0xFF);
}

void Record::write_u8(std::vector<uint8_t>& buf, uint8_t value) {
    buf.push_back(value);
}
