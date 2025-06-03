#include "offset_store/offset_store.h"

class MockOffsetStore : public ConsumerOffsetStore {
public:
    MockOffsetStore() {}
    void save_offset(const std::string& group, const std::string& topic, uint32_t partition, uint64_t offset) override {
        // Do nothing
    }
    std::optional<uint64_t> load_offset(const std::string& group, const std::string& topic, uint32_t partition) override {
        return std::nullopt;
    }
    
};