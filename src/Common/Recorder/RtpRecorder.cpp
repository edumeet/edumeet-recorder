/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#include "RtpRecorder.h"
#include <fmt/core.h>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <utility>

namespace {
auto constexpr CAPS = "application/x-rtp,media=video,clock-rate=90000,encoding-name=VP8,payload=96";
auto constexpr PIPELINE_RTP = "rtpbin name=rtpbin udpsrc caps={} port={} ! rtpbin.recv_rtp_sink_0 udpsrc port={} ! rtpbin.recv_rtcp_sink_0 rtpbin. ! rtpvp8depay ! queue ! webmmux name=mux ! filesink location={}";
}
namespace rec {

CRtpRecorder::CRtpRecorder(std::filesystem::path workdir, const Params& params)
    : IRecorder(workdir)
    , m_params(params)
{
}

bool CRtpRecorder::start()
{
    std::unique_lock lock(m_mtx);
    if (m_pipeline && m_pipeline->isRunning()) {
        return true;
    }
    auto file = genFilename();
    m_recVec.emplace_back(file);
    auto cmd = fmt::format(PIPELINE_RTP, CAPS, m_params.rtpPort, m_params.rtcpPort, file);
    m_pipeline = std::make_unique<CPipeline>(cmd);
    return m_pipeline->start();
}

bool CRtpRecorder::stop()
{
    std::unique_lock lock(m_mtx);
    if (!m_pipeline || (m_pipeline && !m_pipeline->isRunning())) {
        return true;
    }
    return m_pipeline->stop("mux");
}

}