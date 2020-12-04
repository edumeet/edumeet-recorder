/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#include "RecordManager.h"
#include "RtpRecorder.h"
#include "UriRecorder.h"
#include <Config/ConfigParser.h>
#include <filesystem>
#include <iostream>
#include <utility>

namespace rec {

namespace type {
    auto constexpr RTP = "rtp";
    auto constexpr URI = "uri";
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

std::optional<ID> CRecordManager::start(const std::string& params)
{
    try {
        auto data = nlohmann::json::parse(params);
        std::cout << data.dump() << "\n";
        auto type = data.at("type");
        RecPtr rec = nullptr;
        if (type == type::RTP) { //TODO create factory
            rec = rec::create<rec::CRtpRecorder>(m_workDir, data.at("data"));
        }
        else if (type == type::URI) {
            rec = rec::create<rec::CHtmlRecorder>(m_workDir, data.at("data"));
        }
        else {
            return std::nullopt;
        }

        if (rec->start()) {
            return add(std::move(rec));
        }
        else {
            return std::nullopt;
        }
    }
    catch (nlohmann::json::exception& ex) {
        std::cerr << "parse error at byte " << ex.what() << std::endl;
        return std::nullopt;
    }
}

ID CRecordManager::add(CRecordManager::RecPtr&& rec)
{
    std::unique_lock lock(m_mtx);
    static int counter(0);
    const ID id { ++counter };
    m_recorders.try_emplace(id, std::move(rec));
    return id;
}

void CRecordManager::rm(ID id)
{
    std::unique_lock lock(m_mtx);
    m_recorders.erase(id);
}

bool CRecordManager::stop(ID id)
{
    if (!m_recorders.contains(id)) {
        return false;
    }
    //rm(id); //TODO when erase stopped recorders?
    return m_recorders.at(id)->stop();
}

nlohmann::json CRecordManager::status()
{
    nlohmann::json json;
    for (auto& [id, recorder] : m_recorders) {
        json.push_back({ { "id", id.t }, { "running", recorder->isRunning() }, { "records", recorder->getRecords() } });
    }
    return json;
}
nlohmann::json CRecordManager::getRecords(ID id)
{
    if (!m_recorders.contains(id)) {
        return { };
    }
    return m_recorders.at(id)->getRecords();
}

}