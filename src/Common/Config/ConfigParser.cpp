/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#include <fstream>
#include "ConfigParser.h"

CConfigParser &CConfigParser::getInstance() {
    static CConfigParser parser;
    return parser;
}

void CConfigParser::init(const std::string &path) {
    std::ifstream ifs(path);
    m_config = nlohmann::json::parse(ifs);
}

const nlohmann::json &CConfigParser::root() {
    return getInstance().m_config;
}
