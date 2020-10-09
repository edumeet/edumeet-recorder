/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#ifndef RECORDER_SERVICE_IRECORDER_H
#define RECORDER_SERVICE_IRECORDER_H

#include <Pipeline/Pipeline.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <memory>
#include <mutex>

namespace rec {

template <typename T>
auto create(std::filesystem::path workdir, const nlohmann::json& json)
{
    return std::make_unique<T>(workdir, json.get<typename T::Params>());
}

class IRecorder
{
public:
    explicit IRecorder(std::filesystem::path workdir);
    virtual ~IRecorder() = default;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    std::vector<std::filesystem::path> getRecords();
    bool isRunning();

protected:
    std::string genFilename(const std::string& ext);
    std::unique_ptr<CPipeline> m_pipeline;
    std::filesystem::path m_workdir;
    std::vector<std::filesystem::path> m_recVec;
    std::recursive_mutex m_mtx;
};
}

#endif //RECORDER_SERVICE_IRECORDER_H
