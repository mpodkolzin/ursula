#include "producer/partition_worker.h"
#include "record/record.h"

PartitionWorker::PartitionWorker(Partition& partition)
    : partition_(partition), stop_(false) {}

PartitionWorker::~PartitionWorker() {
    stop();
}

void PartitionWorker::start() {
    thread_ = std::thread(&PartitionWorker::run, this);
}

void PartitionWorker::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
    }
    cv_.notify_all();
    if (thread_.joinable()) {
        thread_.join();
    }
}

std::future<uint64_t> PartitionWorker::enqueue(const Record& record) {
    ProduceTask task{record, std::promise<uint64_t>{}};
    std::future<uint64_t> future = task.result_promise.get_future();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(task));
    }

    cv_.notify_one();
    return future;
}

void PartitionWorker::run() {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [&] { return stop_ || !queue_.empty(); });

        if (stop_ && queue_.empty())
            break;

        ProduceTask task = std::move(queue_.front());
        queue_.pop();
        lock.unlock();

        uint64_t offset = partition_.append(task.record);
        task.result_promise.set_value(offset);
    }
}
