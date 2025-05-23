#include "util/crc32.h"
#include <zlib.h>

uint32_t compute_crc32(const std::vector<uint8_t>& data) {
    return ::crc32(0L, reinterpret_cast<const Bytef*>(data.data()), data.size());
}
