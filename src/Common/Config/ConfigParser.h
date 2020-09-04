/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#ifndef RECORDER_SERVICE_CONFIGPARSER_H
#define RECORDER_SERVICE_CONFIGPARSER_H
#include <nlohmann/json.hpp>

class CConfigParser {
public:
    static CConfigParser& getInstance();
    void init(const std::string& path);
    static const nlohmann::json &root();

    CConfigParser(const CConfigParser&) = delete;
    CConfigParser(CConfigParser&&) = delete;
    CConfigParser& operator=(const CConfigParser&) = delete;
    CConfigParser& operator=(CConfigParser&&) = delete;

private:
    ~CConfigParser() = default;
    CConfigParser() = default;
    nlohmann::json m_config;
};

#endif //RECORDER_SERVICE_CONFIGPARSER_H
