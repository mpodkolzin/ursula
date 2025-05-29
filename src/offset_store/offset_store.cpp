#pragma once

#include "consumer_offset_store.h"
#include <fstream>
#include <unordered_map>
#include <mutex>
#include <filesystem>

class FileOffsetStore : public ConsumerOffsetStore {
public:
    explicit FileOffsetStore(const std::string& filename)
        : filename_(filename) {
        load_file();
    }

    void save_offset(const std::string& group,
                     const std::string& topic,
                     uint32_t partition,
                     uint64_t offset) override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string key = make_key(group, topic, partition);
        offsets_[key] = offset;
        flush_file();
    }

    std::optional<uint64_t> load_offset(const std::string& group,
                                        const std::string& topic,
                                        uint32_t partition) override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string key = make_key(group, topic, partition);
        auto it = offsets_.find(key);
        if (it != offsets_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

private:
    std::string filename_;
    std::unordered_map<std::string, uint64_t> offsets_;
    std::mutex mutex_;

    std::string make_key(const std::string& group,
                         const std::string& topic,
                         uint32_t partition) const {
        return group + "." + topic + "." + std::to_string(partition);
    }

    void load_file() {
        if (!std::filesystem::exists(filename_)) return;
        std::ifstream infile(filename_);
        std::string line;
        while (std::getline(infile, line)) {
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string key = line.substr(0, eq);
            uint64_t offset = std::stoull(line.substr(eq + 1));
            offsets_[key] = offset;
        }
    }

    void flush_file() {
        std::ofstream outfile(filename_, std::ios::trunc);
        for (const auto& [key, offset] : offsets_) {
            outfile << key << "=" << offset << "\n";
        }
    }
};
