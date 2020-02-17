/*

    filesentinel

            a helper library that monitors a file for changes on another thread

    (c)2020 Paul Reilly, MIT License

*/

#ifndef FILESENTINEL_H_
#define FILESENTINEL_H_

#include <filesystem>
#include <string>
#include <atomic>
#include <thread>

namespace filesentinel {
    enum class kFileState {
        unchanged = 0,
        modified,
        does_not_exist
    };

    struct FileSentinel
    {
        FileSentinel(
            const std::string path_str,
            const unsigned int interval_ms = 10e2
        );
        ~FileSentinel();
        FileSentinel(const FileSentinel& other) = delete;
        FileSentinel(FileSentinel&& other) noexcept = delete;
        FileSentinel& operator=(const FileSentinel& rhs) = delete;
        FileSentinel& operator=(FileSentinel&& rhs) noexcept = delete;

        auto start() 
        -> bool;

        auto stop() 
        -> void;

        // Returns change state of file. Resets the state of the file to unchanged
        // so this method can only be called usefully once per change.
        [[nodiscard]]
        auto getAndResetFileState() 
        -> const kFileState;

    private:
        // detached thread created on start(), signal sent to terminate on stop() or on destruction
        auto thread_monitorFile(std::shared_ptr<std::atomic<bool>> terminate, 
            const std::filesystem::path path, const unsigned int interval_ms, 
            std::shared_ptr<std::atomic<kFileState>>)
        -> void;

        std::thread m_thread;
        const std::filesystem::path m_path;
        const unsigned int m_interval_ms;
        std::shared_ptr<std::atomic<bool>> spm_terminate_thread;
        std::shared_ptr<std::atomic<kFileState>> spm_file_state;
    };
}

#endif