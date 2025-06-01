#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <regex>
#include <filesystem>
#include "partition/log_segment.h"
#include "partition/partition_writer.h"
#include "partition/partition_reader.h"
#include "record/record.h"
#include "partition/partition.h"
#include <cassert>

Partition::Partition(const std::string& path) : path_(path) {
    load_segments();
    writer_ = std::make_unique<PartitionWriter>(segments_, path_);
    reader_ = std::make_unique<PartitionReader>(segments_);
    assert(writer_);
    assert(reader_);
}

void Partition::load_segments() {
    namespace fs = std::filesystem;
    std::regex log_file_regex(R"((\d{20})\.log)");

    if (!fs::exists(path_)) {
        fs::create_directories(path_);
        return;
    }

    for (const auto& entry : fs::directory_iterator(path_)) {
        std::string filename = entry.path().filename().string();
        std::smatch match;
        if (std::regex_match(filename, match, log_file_regex)) {
            uint64_t base_offset = std::stoull(match[1].str());
            std::string base_path = (entry.path().parent_path() / match[1].str()).string();
            segments_[base_offset] = std::make_unique<LogSegment>(base_path, base_offset);
        }
    }
}

uint64_t Partition::append(const Record& record) {
    assert(writer_);
    return writer_->append(record);
}

Record Partition::read(uint64_t offset) {
    return reader_->read(offset);
}