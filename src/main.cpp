#include "telemetry_streamer.h"
#include <iostream>
#include <thread>

int main(int argc, char* argv[]) {
    std::cout << "Starting Telemetry Cache Streamer Daemon v1.4.2..." << std::endl;
    
    tcs::TelemetryStreamer streamer("telemetry_cache.json", 10);
    
    streamer.log("INFO", "system", "Daemon initialization completed successfully");
    streamer.log("INFO", "buffer_manager", "Bound local cache swap file to telemetry_cache.json");
    
    // Simulate streaming cycles
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    streamer.log("DEBUG", "telemetry_core", "Polled kernel memory stats");
    streamer.log("INFO", "stream_client", "Connection verified to telemetry.local.internal");
    
    streamer.flush();
    std::cout << "Telemetry flush completed. Daemon running." << std::endl;
    return 0;
}
