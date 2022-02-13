/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#ifndef RECORDER_SERVICE_CRECORDMANAGER_H
#define RECORDER_SERVICE_CRECORDMANAGER_H

#include <boost/serialization/strong_typedef.hpp>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <optional>
#include <map>
#include <memory>
#include <mutex>

namespace rec {

class IRecorder;

BOOST_STRONG_TYPEDEF(int, ID)

class CRecordManager
{
public:
    static CRecordManager& getInstance();
    CRecordManager(const CRecordManager&) = delete;
    CRecordManager& operator=(const CRecordManager&) = delete;
    std::optional<ID> start(const std::string& params);
    bool stop(ID id);
    nlohmann::json getRecords(ID id);
    nlohmann::json status();

private:
    CRecordManager();
    ~CRecordManager() = default;
    using RecPtr = std::unique_ptr<rec::IRecorder>;
    ID add(RecPtr&&);
    void rm(ID);
    std::map<ID, RecPtr> m_recorders;
    std::filesystem::path m_workDir;
    std::mutex m_mtx;
};

}
#endif //RECORDER_SERVICE_CRECORDMANAGER_H
