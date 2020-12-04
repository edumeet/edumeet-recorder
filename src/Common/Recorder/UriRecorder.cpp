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
    { rec::EncoderType::H264, { .m_video = "x264enc speed-preset=faster key-int-max=30 bitrate={bitrate} ! video/x-h264,profile=baseline", .m_audio = "voaacenc bitrate=256000", .m_muxer = "mp4mux faststart=true", .m_ext = "mp4" } },
    { rec::EncoderType::VP8, { .m_video = "vp8enc target-bitrate={bitrate} overshoot=25 undershoot=100 deadline=33000 keyframe-max-dist=1", .m_audio = "vorbisenc", .m_muxer = "webmmux", .m_ext = "webm" } },
};

auto constexpr PIPELINE = "wpesrc location={uri} ! video/x-raw,framerate=30/1 ! queue max-size-bytes=0 ! videoconvert ! {videoEncoder} ! queue ! mux. "
                          "{muxer} name=mux ! filesink location={location}.{extension} "
                          "pulsesrc  ! audio/x-raw,format=S16LE,layout=interleaved,rate=48000,channels=2 ! "
                          "queue max-size-bytes=0 max-size-buffers=0 max-size-time=10000000000 ! audioconvert ! audioresample ! {audioEncoder} ! queue ! mux.";
}

namespace rec {

std::string CHtmlRecorder::createCmd(Params params, const std::string&, const std::string& location)
{
    std::string cmd = PIPELINE;
    auto enc = ENCODERS.at(params.encoder);
    auto videoEncoder = fmt::format(enc.m_video, fmt::arg("bitrate", params.bitrate));
    return fmt::format(cmd, fmt::arg("uri", params.uri), fmt::arg("audioEncoder", enc.m_audio),
        fmt::arg("videoEncoder", videoEncoder), fmt::arg("muxer", enc.m_muxer), fmt::arg("location", location),
        fmt::arg("extension", enc.m_ext));
}

Pulse createPulseSink()
{
    static auto count = 0;
    auto name = "v_rec_sink" + std::to_string(++count);
    std::future<std::string> output;
    std::future<std::string> err;
    namespace bp = boost::process;
    const auto ret = bp::system(bp::search_path("pactl"),
        bp::args = { "load-module", "module-null-sink", fmt::format("sink_name={}", name) },
        bp::std_out > output, bp::std_err > err);
    std::string id = output.get();
    if (ret != 0 || id.empty()) {
        std::cerr << fmt::format("Error! Can't read Out Audio device! {}:'{}'", ret, err.get());
        throw std::runtime_error("Error! Can't read Out Audio device!");
    }
    boost::trim(id);
    return { .m_name = name, .m_id = id };
}

CHtmlRecorder::CHtmlRecorder(std::filesystem::path workdir, Params params)
    : IRecorder(std::move(workdir))
    , m_params(std::move(params))
 , m_pulse(createPulseSink())
{
    setenv("recorder_service_pulse_device_name", m_pulse.m_name.c_str(), 1);
}

bool CHtmlRecorder::start()
{
    std::unique_lock lock(m_mtx);
    if (m_pipeline && m_pipeline->isRunning()) {
        return true;
    }
    auto file = genFilename();
    m_recVec.emplace_back(file);
    auto cmd = createCmd(m_params, m_pulse.m_name, file);
    m_pipeline = std::make_unique<CPipeline>(cmd);
    return m_pipeline->start();
}

bool CHtmlRecorder::stop()
{
    std::unique_lock lock(m_mtx);
    if (!m_pipeline || (m_pipeline && !m_pipeline->isRunning())) {
        return true;
    }
    auto ret = m_pipeline->stop();
    namespace bp = boost::process;
    boost::process::system(fmt::format("pactl unload-module {}", m_pulse.m_id));
    return ret;
}

}
