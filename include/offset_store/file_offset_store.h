#include "offset_store/offset_store.h"
#include <filesystem>
#include <mutex>

class FileOffsetStore : public ConsumerOffsetStore {
public:
    FileOffsetStore(const std::string& root_dir);
    void save_offset(const std::string& group, const std::string& topic, uint32_t partition, uint64_t offset) override;
    std::optional<uint64_t> load_offset(const std::string& group, const std::string& topic, uint32_t partition) override;

private:
    std::string root_dir_;
    std::mutex mutex_;

    std::filesystem::path make_path(const std::string& group,
                                                    const std::string& topic,
                                                    uint32_t partition) const;
};
