#pragma once
#include <string>
#include <optional>

class ConsumerOffsetStore {
public:
    virtual void save_offset(const std::string& group,
                             const std::string& topic,
                             uint32_t partition,
                             uint64_t offset) = 0;

    virtual std::optional<uint64_t> load_offset(const std::string& group,
                                                const std::string& topic,
                                                uint32_t partition) = 0;

    virtual ~ConsumerOffsetStore() = default;
};