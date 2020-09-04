/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#ifndef RECORDER_SERVICE_CRECORDMANAGER_H
#define RECORDER_SERVICE_CRECORDMANAGER_H

#include <nlohmann/json.hpp>
#include <filesystem>
#include <map>
#include <memory>
#include <boost/serialization/strong_typedef.hpp>

namespace rec {

class CRecorder;

BOOST_STRONG_TYPEDEF(int, ID)

class CRecordManager
{
public:
    static CRecordManager& getInstance();
    CRecordManager(const CRecordManager&) = delete;
    CRecordManager& operator=(const CRecordManager&) = delete;
    std::optional<ID> startRecording(int rtpPort, int rtcpPort);
    std::optional<std::filesystem::path> stopRecording(ID id);
    nlohmann::json staus();

private:
    CRecordManager();
    ~CRecordManager() = default;
    std::map<ID, std::unique_ptr<rec::CRecorder>> m_recorders;
    std::filesystem::path m_workDir;
};

}
#endif //RECORDER_SERVICE_CRECORDMANAGER_H
