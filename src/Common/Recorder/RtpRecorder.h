/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#ifndef RECORDER_SERVICE_RTP_RECORDER_H
#define RECORDER_SERVICE_RTP_RECORDER_H

#include "IRecorder.h"
#include <nlohmann/json.hpp>

namespace rec {

class CRtpRecorder : public IRecorder
{
public:
    struct Params
    {
        int rtpPort = 0;
        int rtcpPort = 0;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Params, rtpPort, rtcpPort)
    };

    explicit CRtpRecorder(std::filesystem::path workdir, const Params& params);
    bool start() override;
    bool stop() override;
    ~CRtpRecorder() override = default;

private:
    Params m_params;
};
}

#endif //RECORDER_SERVICE_RTP_RECORDER_H
