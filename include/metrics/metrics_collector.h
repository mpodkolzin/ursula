#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>
#include <chrono>

class MetricsCollector {
public:
    MetricsCollector();
    void increment_produced();
    void increment_consumed();

    uint64_t produced() const;
    uint64_t consumed() const;

    double produced_rate() const;  // messages/sec
    double consumed_rate() const;  // messages/sec

private:
    void update_timestamp(std::atomic<std::chrono::steady_clock::time_point>& first,
                        std::atomic<std::chrono::steady_clock::time_point>& last);

    std::atomic<uint64_t> produced_count_{0};
    std::atomic<uint64_t> consumed_count_{0};

    std::chrono::steady_clock::time_point start_time_;
    mutable std::mutex consumed_rate_mutex_;
    mutable std::mutex produced_rate_mutex_;
    std::chrono::steady_clock::time_point produced_last_;
    std::chrono::steady_clock::time_point consumed_last_;
};
