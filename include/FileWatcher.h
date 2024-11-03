#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <iostream>
#include <filesystem>
#include <thread>
#include <functional>
#include <map>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>

enum class FileEventType {
    FileCreated,
    FileModified,
    FileDeleted,
    FileRenamed,
    FileAccessed
};

struct FileEvent {
    std::string path;
    FileEventType type;
};

class FileWatcher {
public:
    void startWatching(const std::string& path);
    void stopWatching(const std::string& path);
    void addWatcher(const std::string& path, std::function<void(FileEvent)> callback);
    void removeWatcher(const std::string& path);
    void setEventFilter(const std::vector<FileEventType>& eventTypes);
    std::vector<std::string> getActiveWatchers();
    void pauseMonitoring();
    void resumeMonitoring();
    void setPollingInterval(std::chrono::milliseconds interval);
    void enableLogging(const std::string& logFilePath);

private:
    void monitor(const std::string& path);
    void logEvent(const FileEvent& event);

    std::map<std::string, std::function<void(FileEvent)>> callbacks;
    std::map<std::string, std::vector<FileEventType>> eventFilters;
    std::map<std::string, std::filesystem::file_time_type> fileTimes;
    std::vector<std::string> watchers;

    // New member variables
    bool isPaused = false;
    std::chrono::milliseconds pollingInterval = std::chrono::milliseconds(1000); // Default polling interval
    std::ofstream logFile;
    bool loggingEnabled = false;
};

#endif // FILEWATCHER_H