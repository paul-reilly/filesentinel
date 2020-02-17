#ifndef DOCTEST_CONFIG_DISABLE
 #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#else
 #define DOCTEST_CONFIG_IMPLEMENT
#endif

#include "filesentinel.hpp"
#include <chrono>

namespace filesentinel {
    namespace fs = std::filesystem;

    FileSentinel::FileSentinel(
        const std::string path_str,
        unsigned int interval_ms
    )
        : m_path(std::filesystem::path(path_str))
        , m_interval_ms(interval_ms)
        , spm_file_state(std::make_shared<std::atomic<kFileState>>(kFileState::unchanged))
        , spm_terminate_thread(std::make_shared<std::atomic<bool>>(false))
    {}

    FileSentinel::~FileSentinel()
    {
        // notify detached thread
        *spm_terminate_thread = true;
    }

    auto FileSentinel::start()
    -> bool
    {
        m_thread = std::thread( &FileSentinel::thread_monitorFile, this, 
            spm_terminate_thread, m_path, m_interval_ms, spm_file_state);

        m_thread.detach();
        return true;
    }

    auto FileSentinel::stop()
    -> void
    {
        *spm_terminate_thread = true;
    }

    auto FileSentinel::getAndResetFileState()
    -> const kFileState
    {
        auto tmp = spm_file_state->load();
        if (*spm_file_state == kFileState::modified)
            *spm_file_state = kFileState::unchanged;
        return tmp;
    }

    // this thread potentially outlives the creating object
    auto FileSentinel::thread_monitorFile(std::shared_ptr<std::atomic<bool>> sp_terminate,
        const fs::path path, unsigned int interval_ms, std::shared_ptr<std::atomic<kFileState>> sp_file_state)
    -> void
    {
        if (!fs::exists(path)) {
            *sp_file_state = kFileState::does_not_exist;
        }
        auto last_write_time = fs::last_write_time(path);

        while (!*sp_terminate) {
            if (!fs::exists(path)) {
                *sp_file_state = kFileState::does_not_exist;
            }
            if (last_write_time != fs::last_write_time(path)) {
                last_write_time = fs::last_write_time(path);
                *sp_file_state = kFileState::modified;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
        }
    }
}

#ifndef DOCTEST_CONFIG_DISABLE 
#include "../doctest.h"

TEST_CASE("Test") {
    INFO("Output");
}

#endif
