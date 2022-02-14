/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
 */

#include "UriRecorder.h"
#include <fmt/core.h>
#include <iomanip>
#include <iostream>
#include <utility>
namespace {
auto constexpr VIDEO_ENCODER = "x264enc speed-preset=superfast key-int-max=60 bitrate={bitrate} ! video/x-h264,profile=baseline";
auto constexpr AUDIO_ENCODER = "avenc_aac bitrate=256000";

auto constexpr RTMP_SINK = "vt. ! queue ! h264parse ! flvmux streamable=true name=flv ! rtmp2sink location={} at. ! queue ! aacparse ! flv.";
auto constexpr FILE_SINK = "vt. ! queue ! mpegtsmux name=mux ! filesink location={location}.ts at. ! queue ! mux.";

auto constexpr PIPELINE = "cefbin name=cef cefsrc::url={uri} cef.video ! video/x-raw,width=1920,height=1080,framerate=30/1 ! "
                          "queue max-size-bytes=0 max-size-buffers=0 max-size-time=3000000000 ! videoconvert ! {videoEncoder} ! tee name=vt "
                          "audiotestsrc do-timestamp=true is-live=true volume=0.0 ! audiomixer name=mix ! "
                          "queue max-size-bytes=0 max-size-buffers=0 max-size-time=3000000000 ! "
                          "audioconvert ! audiorate ! audioresample ! queue ! {audioEncoder} ! tee name=at "
                          "cef.audio ! mix. "
                          "{sink}";
}

namespace rec {

std::string CHtmlEncoder::createCmd(const Params& params, const std::string& location)
{
    const auto rtmpSink = fmt::format(RTMP_SINK, params.stream_uri);
    const auto fileSink = fmt::format(FILE_SINK, fmt::arg("location", location)); // TODO convert to mp4 if needed

    std::string sink {};
    switch (params.mode) {
    case (EncoderMode::STREAM):
        sink = rtmpSink;
        break;
    case (EncoderMode::RECORD):
        sink = fileSink;
        break;
    case (EncoderMode::BOTH):
        sink = fmt::format("{} {}", rtmpSink, fileSink);
        break;
    }

    auto videoEncoder = fmt::format(VIDEO_ENCODER, fmt::arg("bitrate", params.bitrate));
    return fmt::format(PIPELINE, fmt::arg("uri", params.uri), fmt::arg("audioEncoder", AUDIO_ENCODER),
        fmt::arg("videoEncoder", videoEncoder), fmt::arg("sink", sink));
}

CHtmlEncoder::CHtmlEncoder(std::filesystem::path workdir, Params params)
    : IRecorder(std::move(workdir))
    , m_params(std::move(params))
{
}

bool CHtmlEncoder::start()
{
    std::unique_lock lock(m_mtx);
    if (m_pipeline && m_pipeline->isRunning()) {
        return true;
    }
    auto file = genFilename();
    m_recVec.emplace_back(file);
    auto cmd = createCmd(m_params, file);
    std::cout << cmd;
    try {
        m_pipeline = std::make_unique<CPipeline>(cmd);
        return m_pipeline->start();
    }
    catch (std::exception& ex) {
        std::cout << fmt::format("Pipeline failed. Exception: {} \n", ex.what());
        return false;
    }
}

bool CHtmlEncoder::stop()
{
    std::unique_lock lock(m_mtx);
    if (!m_pipeline || (m_pipeline && !m_pipeline->isRunning())) {
        return true;
    }
    return m_pipeline->stop();
}

}
