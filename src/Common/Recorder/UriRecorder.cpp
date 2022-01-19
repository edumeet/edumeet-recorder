/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
 */

#include "UriRecorder.h"
#include <boost/process.hpp>
#include <fmt/core.h>
#include <iomanip>
#include <iostream>
#include <utility>
namespace {
struct Encoder
{
    std::string m_video;
    std::string m_audio;
    std::string m_muxer;
    std::string m_ext;
};

std::map<rec::EncoderType, Encoder> ENCODERS = {
    { rec::EncoderType::H264, { .m_video = "x264enc speed-preset=superfast tune=zerolatency key-int-max=60 bitrate={bitrate} ! video/x-h264,profile=baseline", .m_audio = "avenc_aac bitrate=256000", .m_muxer = "mp4mux faststart=true", .m_ext = "mp4" } },
    { rec::EncoderType::VP8, { .m_video = "vp8enc target-bitrate={bitrate} overshoot=25 undershoot=100 deadline=33000 keyframe-max-dist=1", .m_audio = "vorbisenc", .m_muxer = "webmmux", .m_ext = "webm" } },
};

auto constexpr RTMP_SINK = "vt. ! queue ! flvmux name=flv ! rtmp2sink location={} at. ! queue ! flv.";
auto constexpr FILE_SINK = "vt. ! queue ! {muxer} name=mux ! filesink location={location}.{extension} at. ! queue ! mux.";

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
    std::string rtmpSink {};
    if ((params.mode != EncoderMode::RECORD) && (params.encoder != rec::EncoderType::H264)) {
        return {};
    }
    else {
        rtmpSink = fmt::format(RTMP_SINK, params.stream_uri);
    }

    auto enc = ENCODERS.at(params.encoder);
    // auto fileSink = fmt::format(FILE_SINK, fmt::arg("muxer", enc.m_muxer), fmt::arg("location", location), fmt::arg("extension", enc.m_ext));
    auto fileSink = fmt::format(FILE_SINK, fmt::arg("muxer", "mpegtsmux"), fmt::arg("location", location), fmt::arg("extension", "ts")); // TODO convert to mp4 if needed

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

    auto videoEncoder = fmt::format(enc.m_video, fmt::arg("bitrate", params.bitrate));
    return fmt::format(PIPELINE, fmt::arg("uri", params.uri), fmt::arg("audioEncoder", enc.m_audio),
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
