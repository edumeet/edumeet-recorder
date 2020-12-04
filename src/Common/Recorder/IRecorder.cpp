/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#include "IRecorder.h"

namespace rec {
std::string IRecorder::genFilename()
{
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    static std::atomic<int> counter(0);
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d_%H:%M:%S") << "__" << std::to_string(counter++);
    return m_workdir / ss.str();
}

IRecorder::IRecorder(std::filesystem::path workdir)
    : m_workdir(std::move(workdir))
{
    if (!m_workdir.empty() && !std::filesystem::exists(m_workdir)) {
        if (!std::filesystem::create_directories(m_workdir)) {
            throw std::runtime_error("Cannot create workdir.");
        }
    }
}

bool IRecorder::isRunning()
{
    std::unique_lock lock(m_mtx);
    if (!m_pipeline) {
        return false;
    }
    return m_pipeline->isRunning();
}

std::vector<std::filesystem::path> IRecorder::getRecords()
{
    std::unique_lock lock(m_mtx);
    if (m_pipeline && m_pipeline->isRunning()) {
        return {};
    }
    return m_recVec;
}

}
