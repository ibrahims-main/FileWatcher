#include <iostream>
#include "include/FileWatcher.h"

void onFileEvent(FileEvent event) {
    std::cout << "Event occurred: " << static_cast<int>(event.type) 
              << " on file: " << event.path << std::endl;
}

int main() {
    FileWatcher fileWatcher;

    // Enable logging
    fileWatcher.enableLogging("file_events.log");

    // Set a polling interval
    fileWatcher.setPollingInterval(std::chrono::milliseconds(500));

    // Start watching a directory
    std::string directoryToWatch = "./watched_directory";
    fileWatcher.startWatching(directoryToWatch);

    // Add a watcher with a callback for a specific file
    std::string fileToWatch = directoryToWatch + "/example.txt";
    fileWatcher.addWatcher(fileToWatch, onFileEvent);

    // Set an event filter to only watch for file modifications
    fileWatcher.setEventFilter({ FileEventType::FileModified });

    // Simulate some waiting time to allow events to be detected
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Pause monitoring
    fileWatcher.pauseMonitoring();
    std::cout << "Monitoring paused. Waiting for 5 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Resume monitoring
    fileWatcher.resumeMonitoring();

    // Simulate more waiting time to allow events to be detected
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Stop watching the specific file
    fileWatcher.removeWatcher(fileToWatch);

    // Stop watching the directory
    fileWatcher.stopWatching(directoryToWatch);

    return 0;
}