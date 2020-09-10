/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#include "UriRecorder.h"
#include <fmt/core.h>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <utility>

namespace {
auto constexpr PIPELINE_WPE = "wpesrc location={} ! video/x-raw,framerate=30/1 ! queue ! videoconvert ! x264enc bitrate={} speed-preset=ultrafast key-int-max=30 ! video/x-h264,profile=baseline ! queue ! mpegtsmux name=mux ! filesink location={}";
}
namespace rec {

CHtmlRecorder::CHtmlRecorder(std::filesystem::path workdir, Params  params)
    : IRecorder(std::move(workdir))
    , m_params(std::move(params))
{
}

bool CHtmlRecorder::start()
{
    std::unique_lock lock(m_mtx);
    if (m_pipeline && m_pipeline->isRunning()) {
        return true;
    }
    auto file = genFilename("ts");
    m_recVec.emplace_back(file);
    auto cmd = fmt::format(PIPELINE_WPE, m_params.uri, m_params.bitrate, file);
    m_pipeline = std::make_unique<CPipeline>(cmd);
    return m_pipeline->start();
}

}
