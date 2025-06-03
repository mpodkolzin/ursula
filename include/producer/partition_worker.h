#pragma once

#include <future>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "partition/partition.h"

struct ProduceTask {
    Record record;
    std::promise<uint64_t> result_promise;
};

class PartitionWorker {
public:
    PartitionWorker(Partition& partition);

    ~PartitionWorker();

    std::future<uint64_t> enqueue(const Record& record) ;
    void stop();
    void start();

private:
    void run();

    Partition& partition_;
    std::queue<ProduceTask> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread thread_;
    bool stop_;
};
