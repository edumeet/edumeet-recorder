/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#ifndef RECORDER_SERVICE_RECORDER_H
#define RECORDER_SERVICE_RECORDER_H

#include <Pipeline/Pipeline.h>
#include <filesystem>
#include <memory>
#include <mutex>

namespace rec {

class CRecorder
{
public:
    explicit CRecorder(std::filesystem::path  workdir);
    bool start(int rtpPort, int rtcpPort);
    bool start(const std::string& uri);
    bool isRunning();
    std::tuple<bool, std::filesystem::path> stop();
    virtual ~CRecorder();

private:
    std::string genFilename(const std::string& ext);
    std::unique_ptr<CPipeline> m_pipeline;
    std::filesystem::path m_workdir;
    std::filesystem::path m_recPath;
    std::mutex m_mtx;
};
}

#endif //RECORDER_SERVICE_RECORDER_H
