# FileWatcher

A simple and efficient C++ library to monitor file system changes, allowing you to detect modifications, additions, deletions, and renaming of files and directories.

## Features
- Monitors directories and specific files for changes.
- Supports customizable callbacks for different file events (e.g., created, modified, deleted).
- Thread-safe operations.
- Configurable polling interval for monitoring changes.
- Optional logging of file events to a log file.
- Ability to pause and resume monitoring.

## Installation

### Prerequisites

Make sure you have the following installed on your system:

- A C++ compiler (e.g., `g++` or `clang++`)
- CMake (optional, if you want to use CMake for building)
- A text editor or IDE (e.g., Visual Studio Code, Code::Blocks, or any other IDE)

### Clone the repository

To use this library, clone the repository to your local machine using the following command:

```bash
git clone https://github.com/yourusername/FileWatcher.git
```

## Build Instructions

- Copy the Files: Include the `FileWatcher.h` and `FileWatcher.cpp` files in your project.
- Make sure to add your log output file if logging is enabled, or it will be created automatically when the program runs.

## Compiling with FileWatcher

You can compile your code with the FileWatcher library like this:

```bash
g++ -o my_program main.cpp src/FileWatcher.cpp -std=c++11 -pthread
```

## Usage

### Example Code

Here’s a simple example of how to use the `FileWatcher` library in your project:

```c++
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
```

## Example Output in `file_events.log`

If you run the above code, `file_events.log` may contain something like this:

```vbnet
Event occurred: 1 on file: ./watched_directory/example.txt
```

## Customizing the FileWatcher

- *Callbacks*: You can customize the behavior for specific events by providing your own callback functions.
- *Polling Interval*: Adjust the polling interval using `fileWatcher.setPollingInterval()` to fit your needs.
- Event Filtering: Set event filters with `fileWatcher.setEventFilter()` to monitor only specific types of events (e.g., file modifications).

## License

This project is licensed under the [MIT License](LICENSE) - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests.

Happy file watching with FileWatcher!