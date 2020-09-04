/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#include "RecordManager.h"
#include "Recorder.h"
#include <Config/ConfigParser.h>
#include <filesystem>
#include <iostream>

namespace rec {

namespace {
    static int REC_COUNTER = 0;
}

CRecordManager::CRecordManager()
    : m_workDir(CConfigParser::root()["/config/recorder/record_location"_json_pointer].get<std::string>())
{
}

CRecordManager& CRecordManager::getInstance()
{
    static CRecordManager manager;
    return manager;
}

std::optional<ID> CRecordManager::startRecording(int rtpPort, int rtcpPort)
{
    if (auto rec = std::make_unique<rec::CRecorder>(m_workDir); rec->start(rtpPort, rtcpPort)) {
        ID id(++REC_COUNTER);
        m_recorders.try_emplace(id, std::move(rec));
        return id;
    }
    else {
        return std::nullopt;
    }
}

std::optional<std::filesystem::path> CRecordManager::stopRecording(ID id)
{
    if (!m_recorders.contains(id)) {
        return std::nullopt;
    }
    auto [status, path] = m_recorders.at(id)->stop();
    if (status) {
        //m_recorders.erase(id); //TODO when erase stopped recorders?
        return path;
    }
    else {
        std::cout << "stopRecording error\n";
        return std::nullopt;
    }
}

nlohmann::json CRecordManager::staus()
{
    nlohmann::json json;
    for (auto& [id, recorder] : m_recorders) {
        json.push_back({ { "id", id.t }, { "running", recorder->isRunning() } });
    }
    return json;
}

}