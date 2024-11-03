#include "../include/FileWatcher.h"

void FileWatcher::startWatching(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Path does not exist: " << path << std::endl;
        return;
    }

    if (std::find(watchers.begin(), watchers.end(), path) != watchers.end()) {
        std::cerr << "Already watching: " << path << std::endl;
        return;
    }

    // Start monitoring in a new thread
    watchers.push_back(path);
    std::thread(&FileWatcher::monitor, this, path).detach(); // Use detach to run monitor in the background
}

void FileWatcher::stopWatching(const std::string& path) {
    auto it = std::find(watchers.begin(), watchers.end(), path);
    if (it == watchers.end()) {
        std::cerr << "Not watching: " << path << std::endl;
        return;
    }

    // Remove the watcher
    watchers.erase(it);
    callbacks.erase(path); // Also remove associated callbacks
    eventFilters.erase(path); // Remove event filters
}

void FileWatcher::addWatcher(const std::string& path, std::function<void(FileEvent)> callback) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Path does not exist: " << path << std::endl;
        return;
    }

    callbacks[path] = callback; // Store the callback
}

void FileWatcher::removeWatcher(const std::string& path) {
    if (callbacks.find(path) == callbacks.end()) {
        std::cerr << "No callback registered for: " << path << std::endl;
        return;
    }

    callbacks.erase(path); // Remove the callback
}

void FileWatcher::setEventFilter(const std::vector<FileEventType>& eventTypes) {
    // Set the event filter for the currently active watchers
    for (const auto& watcher : watchers) {
        eventFilters[watcher] = eventTypes; // Store the event types for each watcher
    }
}

std::vector<std::string> FileWatcher::getActiveWatchers() {
    return watchers; // Return a copy of the active watchers
}

void FileWatcher::pauseMonitoring() {
    isPaused = true;
    std::cout << "Monitoring paused." << std::endl;
}

void FileWatcher::resumeMonitoring() {
    isPaused = false;
    std::cout << "Monitoring resumed." << std::endl;
}

void FileWatcher::setPollingInterval(std::chrono::milliseconds interval) {
    pollingInterval = interval;
    std::cout << "Polling interval set to " << interval.count() << " milliseconds." << std::endl;
}

void FileWatcher::enableLogging(const std::string& logFilePath) {
    logFile.open(logFilePath, std::ios::app); // Open log file in append mode
    if (logFile.is_open()) {
        loggingEnabled = true;
        std::cout << "Logging enabled to " << logFilePath << std::endl;
    } else {
        std::cerr << "Failed to open log file: " << logFilePath << std::endl;
    }
}

void FileWatcher::logEvent(const FileEvent& event) {
    if (loggingEnabled && logFile.is_open()) {
        logFile << "Event: " << static_cast<int>(event.type) << ", File: " << event.path << std::endl;
    }
}

void FileWatcher::monitor(const std::string& path) {
    while (true) {
        if (isPaused) {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep if paused
            continue; // Skip the rest of the loop
        }

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            auto currentTime = std::filesystem::last_write_time(entry.path());

            // Check for file creation or modification
            if (fileTimes.find(entry.path().string()) == fileTimes.end()) {
                // File created
                fileTimes[entry.path().string()] = currentTime;
                std::cout << "File created: " << entry.path().string() << std::endl;

                // Trigger the callback if it exists and the event type is allowed
                if (callbacks.find(entry.path().string()) != callbacks.end()) {
                    FileEvent event{ entry.path().string(), FileEventType::FileCreated };
                    // Check if the event type is allowed
                    if (eventFilters[path].empty() || std::find(eventFilters[path].begin(), eventFilters[path].end(), FileEventType::FileCreated) != eventFilters[path].end()) {
                        callbacks[entry.path().string()](event);
                        logEvent(event); // Log the event
                    }
                }
            } else if (fileTimes[entry.path().string()] != currentTime) {
                // File modified
                fileTimes[entry.path().string()] = currentTime;
                std::cout << "File modified: " << entry.path().string() << std::endl;

                // Trigger the callback if it exists and the event type is allowed
                if (callbacks.find(entry.path().string()) != callbacks.end()) {
                    FileEvent event{ entry.path().string(), FileEventType::FileModified };
                    // Check if the event type is allowed
                    if (eventFilters[path].empty() || std::find(eventFilters[path].begin(), eventFilters[path].end(), FileEventType::FileModified) != eventFilters[path].end()) {
                        callbacks[entry.path().string()](event);
                        logEvent(event); // Log the event
                    }
                }
            }
        }

        // Check for file deletion
        for (auto it = fileTimes.begin(); it != fileTimes.end();) {
            if (!std::filesystem::exists(it->first)) {
                std::cout << "File deleted: " << it->first << std::endl;

                // Trigger the callback if it exists and the event type is allowed
                if (callbacks.find(it->first) != callbacks.end()) {
                    FileEvent event{ it->first, FileEventType::FileDeleted };
                    // Check if the event type is allowed
                    if (eventFilters[path].empty() || std::find(eventFilters[path].begin(), eventFilters[path].end(), FileEventType::FileDeleted) != eventFilters[path].end()) {
                        callbacks[it->first](event);
                        logEvent(event); // Log the event
                    }
                }

                it = fileTimes.erase(it); // Remove from map
            } else {
                ++it;
            }
        }

        std::this_thread::sleep_for(pollingInterval); // Use customized polling interval
    }
}