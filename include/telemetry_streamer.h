#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <queue>

namespace tcs {

struct LogEntry {
    std::string timestamp;
    std::string level;
    std::string component;
    std::string message;
};

struct TelemetryMetrics {
    double buffer_usage_percentage;
    int total_packets_buffered;
    int active_stream_workers;
    double avg_latency_ms;
};

class TelemetryStreamer {
public:
    TelemetryStreamer(const std::string& cache_file_path, size_t max_buffer_size);
    ~TelemetryStreamer();

    void log(const std::string& level, const std::string& component, const std::string& message);
    void flush();
    TelemetryMetrics get_metrics() const;

private:
    void write_to_disk();
    std::string get_current_timestamp() const;

    std::string cache_file_path_;
    size_t max_buffer_size_;
    std::queue<LogEntry> log_queue_;
    mutable std::mutex mutex_;
    
    // Metrics
    size_t total_packets_buffered_{0};
    size_t active_workers_{4};
    double avg_latency_ms_{12.4};
};

} // namespace tcs
