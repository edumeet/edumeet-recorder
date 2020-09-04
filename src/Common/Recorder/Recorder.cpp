/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#include "Recorder.h"
#include <fmt/core.h>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <utility>

namespace {
auto constexpr CAPS = "application/x-rtp,media=video,clock-rate=90000,encoding-name=VP8,payload=96";
auto constexpr PIPELINE_RTP = "rtpbin name=rtpbin udpsrc caps={} port={} ! rtpbin.recv_rtp_sink_0 udpsrc port={} ! rtpbin.recv_rtcp_sink_0 rtpbin. ! rtpvp8depay ! queue ! webmmux name=mux ! filesink location={}";
auto constexpr PIPELINE_WPE = "wpesrc location={} ! queue ! gldownload ! videoconvert ! queue ! x264enc bitrate=6000 speed-preset=ultrafast key-int-max=30 ! video/x-h264,profile=baseline ! queue ! mpegtsmux ! filesink location={}";
}
namespace rec {
std::string CRecorder::genFilename(const std::string& ext)
{
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    static std::atomic<int> counter(0);
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d_%H:%M:%S") << "__" << std::to_string(counter++) << "." << ext;
    return m_workdir / ss.str();
}

CRecorder::CRecorder(std::filesystem::path workdir)
    : m_workdir(std::move(workdir))
{
    if (!m_workdir.empty() && !std::filesystem::exists(m_workdir)) {
        if (!std::filesystem::create_directories(m_workdir)) {
            throw std::runtime_error("Cannot create workdir.");
        }
    }
}

CRecorder::~CRecorder()
{
    stop();
}

bool CRecorder::start(const std::string& uri)
{
    std::unique_lock lock(m_mtx);
    if (m_pipeline && m_pipeline->isRunning()) {
        return true;
    }
    m_recPath = genFilename("ts");
    auto cmd = fmt::format(PIPELINE_WPE, uri, m_recPath.string());
    m_pipeline = std::make_unique<CPipeline>(cmd);
    return m_pipeline->start();
}

bool CRecorder::start(int rtpPort, int rtcpPort)
{
    std::unique_lock lock(m_mtx);
    if (m_pipeline && m_pipeline->isRunning()) {
        return true;
    }
    m_recPath = genFilename("webm");
    auto cmd = fmt::format(PIPELINE_RTP, CAPS, rtpPort, rtcpPort, m_recPath.string());
    m_pipeline = std::make_unique<CPipeline>(cmd);
    return m_pipeline->start();
}

std::tuple<bool, std::filesystem::path> CRecorder::stop()
{
    std::unique_lock lock(m_mtx);
    if (!m_pipeline || (m_pipeline && !m_pipeline->isRunning())) {
        return { true, m_recPath };
    }
    return { m_pipeline->stop("mux"), m_recPath };
}
bool CRecorder::isRunning()
{
    std::unique_lock lock(m_mtx);
    if (!m_pipeline) {
        return false;
    }
    return m_pipeline->isRunning();
}

}
