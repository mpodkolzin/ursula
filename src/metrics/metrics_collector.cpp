#include "metrics/metrics_collector.h"

MetricsCollector::MetricsCollector()
    : start_time_(std::chrono::steady_clock::now()),
      produced_last_(start_time_),
      consumed_last_(start_time_) {}

void MetricsCollector::increment_produced() {
    produced_count_++;
    std::lock_guard<std::mutex> lock(produced_rate_mutex_);
    produced_last_ = std::chrono::steady_clock::now();
}

void MetricsCollector::increment_consumed() {
    consumed_count_++;
    std::lock_guard<std::mutex> lock(consumed_rate_mutex_);
    consumed_last_ = std::chrono::steady_clock::now();
}

uint64_t MetricsCollector::produced() const {
    return produced_count_.load();
}

uint64_t MetricsCollector::consumed() const {
    return consumed_count_.load();
}

double MetricsCollector::produced_rate() const {
    std::lock_guard<std::mutex> lock(produced_rate_mutex_);
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start_time_).count();
    return duration > 0 ? static_cast<double>(produced()) / duration : 0.0;
}

double MetricsCollector::consumed_rate() const {
    std::lock_guard<std::mutex> lock(consumed_rate_mutex_);
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start_time_).count();
    return duration > 0 ? static_cast<double>(consumed()) / duration : 0.0;
}
