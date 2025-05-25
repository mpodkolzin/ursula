#include "io/file_handle.h"
#include "record/record.h"
#include "io/record_reader.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <stdexcept>
#include <spdlog/spdlog.h>
RecordReader::RecordReader(FileHandle& file) : file_(file) {}

Record RecordReader::read_at(uint32_t offset) {
    file_.seek(offset, SEEK_SET);

    std::vector<uint8_t> buffer(4096);  // Read a chunk into a buffer
    ssize_t bytes_read = file_.read(buffer.data(), buffer.size());
    if (bytes_read <= 0) {
        throw std::runtime_error("Failed to read from log file");
    }

    buffer.resize(bytes_read);
    size_t pos = 0;
    return Record::deserialize(buffer, pos);
}

