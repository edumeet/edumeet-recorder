/* Copyright (C) 2020
 *
 * Authors: TT Wojciech Kapsa
*/

#ifndef RECORDER_SERVICE_HTML_RECORDER_H
#define RECORDER_SERVICE_HTML_RECORDER_H

#include "IRecorder.h"
#include <nlohmann/json.hpp>

namespace rec {

struct Pulse
{
    std::string m_name;
    std::string m_id;
};

class CHtmlRecorder : public IRecorder
{
public:
    struct Params
    {
        int bitrate = 0;
        std::string uri = {};
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Params, bitrate, uri)
    };

    explicit CHtmlRecorder(std::filesystem::path workdir, Params params);
    bool start() override;
    bool stop() override;
    ~CHtmlRecorder() override = default;

private:
    Params m_params;
    const Pulse m_pulse;
};

}

#endif //RECORDER_SERVICE_HTML_RECORDER_H
