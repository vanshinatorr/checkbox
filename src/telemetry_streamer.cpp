#include "telemetry_streamer.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace tcs {

TelemetryStreamer::TelemetryStreamer(const std::string& cache_file_path, size_t max_buffer_size)
    : cache_file_path_(cache_file_path), max_buffer_size_(max_buffer_size) {}

TelemetryStreamer::~TelemetryStreamer() {
    flush();
}

void TelemetryStreamer::log(const std::string& level, const std::string& component, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    LogEntry entry{
        get_current_timestamp(),
        level,
        component,
        message
    };

    log_queue_.push(entry);
    total_packets_buffered_++;

    if (log_queue_.size() > max_buffer_size_) {
        write_to_disk();
    }
}

void TelemetryStreamer::flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!log_queue_.empty()) {
        write_to_disk();
    }
}

TelemetryMetrics TelemetryStreamer::get_metrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    double usage = (static_cast<double>(log_queue_.size()) / max_buffer_size_) * 100.0;
    return TelemetryMetrics{
        usage,
        static_cast<int>(total_packets_buffered_),
        static_cast<int>(active_workers_),
        avg_latency_ms_
    };
}

void TelemetryStreamer::write_to_disk() {
    std::ofstream file(cache_file_path_, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Failed to open swap cache file: " << cache_file_path_ << std::endl;
        return;
    }

    double usage = (static_cast<double>(log_queue_.size()) / max_buffer_size_) * 100.0;

    file << "{\n";
    file << "  \"status\": \"active\",\n";
    file << "  \"engine_version\": \"1.4.2\",\n";
    file << "  \"last_telemetry_flush\": \"" << get_current_timestamp() << "\",\n";
    file << "  \"metrics\": {\n";
    file << "    \"buffer_usage_percentage\": " << std::fixed << std::setprecision(1) << usage << ",\n";
    file << "    \"total_packets_buffered\": " << total_packets_buffered_ << ",\n";
    file << "    \"active_stream_workers\": " << active_workers_ << ",\n";
    file << "    \"avg_latency_ms\": " << avg_latency_ms_ << "\n";
    file << "  },\n";
    file << "  \"log_queue\": [\n";

    std::vector<LogEntry> temp_logs;
    while (!log_queue_.empty()) {
        temp_logs.push_back(log_queue_.front());
        log_queue_.pop();
    }

    for (size_t i = 0; i < temp_logs.size(); ++i) {
        file << "    {\n";
        file << "      \"timestamp\": \"" << temp_logs[i].timestamp << "\",\n";
        file << "      \"level\": \"" << temp_logs[i].level << "\",\n";
        file << "      \"component\": \"" << temp_logs[i].component << "\",\n";
        file << "      \"message\": \"" << temp_logs[i].message << "\"\n";
        file << "    }" << (i == temp_logs.size() - 1 ? "" : ",") << "\n";
    }

    file << "  ]\n";
    file << "}\n";
}

std::string TelemetryStreamer::get_current_timestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

} // namespace tcs
