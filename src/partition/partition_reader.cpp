#include "partition/partition_reader.h"
#include <filesystem>
#include <stdexcept>
#include <cstring>
#include <fcntl.h>
#include "record/record.h"
#include "spdlog/spdlog.h"

PartitionReader::PartitionReader(const std::string& segment_dir)
    : segment_dir_(segment_dir)
{
    // Get latest segment base offset by parsing the directory (simplified)
    for (const auto& entry : std::filesystem::directory_iterator(segment_dir_)) {
        auto name = entry.path().filename().string();
        if (name.ends_with(".log")) {
            uint64_t candidate_offset = std::stoull(name.substr(0, name.size() - 4));
            if (candidate_offset >= base_offset_) {
                base_offset_ = candidate_offset;
            }
        }
    }

    std::string base_name = segment_dir_ + "/" + std::to_string(base_offset_);
    log_file_ = std::make_unique<FileHandle>(base_name + ".log", O_RDONLY);
    index_file_ = std::make_unique<FileHandle>(base_name + ".index", O_RDONLY);
}

uint32_t PartitionReader::find_log_position(uint64_t offset) {
    uint32_t relative = static_cast<uint32_t>(offset - base_offset_);
    uint32_t ro, pos;
    size_t entry_size = sizeof(uint32_t) * 2;

    off_t index_size = std::filesystem::file_size(segment_dir_ + "/" + std::to_string(base_offset_) + ".index");
    size_t entry_count = index_size / entry_size;

    spdlog::info("Looking for offset {}, relative {}, entries {}", offset, relative, entry_count);


    for (size_t i = 0; i < entry_count; ++i) {
        index_file_->seek(i * entry_size, SEEK_SET);
        index_file_->read(&ro, sizeof(ro));
        index_file_->read(&pos, sizeof(pos));
        spdlog::info("Index entry {}: rel_offset={}, log_pos={}", i, ro, pos);
        if (ro == relative) {
            return pos;
        }
    }

    throw std::runtime_error("Offset not found in index: " + std::to_string(offset));
}

Record PartitionReader::read(uint64_t offset) {
    uint32_t log_pos = find_log_position(offset);
    log_file_->seek(log_pos, SEEK_SET);

    std::vector<uint8_t> buffer(4096);
    ssize_t bytes_read = log_file_->read(buffer.data(), buffer.size());
    if (bytes_read <= 0) {
        throw std::runtime_error("Failed to read from log file");
    }

    buffer.resize(bytes_read);
    size_t pos = 0;
    return Record::deserialize(buffer, pos);
}
