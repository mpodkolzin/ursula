#include "offset_store/offset_store.h"
#include "offset_store/file_offset_store.h"
#include <sstream>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>


FileOffsetStore::FileOffsetStore(const std::string& root_dir) : root_dir_(root_dir) {
        std::filesystem::create_directories(root_dir_);
    }

void FileOffsetStore::save_offset(const std::string& group, const std::string& topic,
                                    uint32_t partition,
                                    uint64_t offset) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto path = make_path(group, topic, partition);
        std::filesystem::create_directories(path.parent_path());
        std::ofstream ofs(path);
        if (!ofs) {
            spdlog::error("Failed to write offset to file: {}", path.string());
            return;
        }
    ofs << offset;
}

std::optional<uint64_t> FileOffsetStore::load_offset(const std::string& group,
                                                        const std::string& topic,
                                                        uint32_t partition) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto path = make_path(group, topic, partition);
        std::ifstream ifs(path);
        if (!ifs) return std::nullopt;
        uint64_t offset;
        ifs >> offset;
        return offset;
    }


std::filesystem::path FileOffsetStore::make_path(const std::string& group,
                                                    const std::string& topic,
                                                    uint32_t partition) const {
    std::ostringstream oss;
    oss << group << "/" << topic << "/" << partition << ".offset";
    return std::filesystem::path(root_dir_) / oss.str();
}
