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
auto constexpr PIPELINE_WPE = "wpesrc location={} ! video/x-raw,framerate=30/1 ! queue max-size-bytes=0 ! videoconvert ! x264enc bitrate={} speed-preset=faster key-int-max=30 ! video/x-h264,profile=baseline ! queue ! mux. "
                              "mpegtsmux name=mux ! filesink location={} "
                              "pulsesrc device={}.monitor ! audio/x-raw,format=S16LE,layout=interleaved,rate=48000,channels=2 ! queue max-size-bytes=0 max-size-buffers=0 max-size-time=10000000000 ! audioconvert ! audioresample ! opusenc ! queue ! mux.";
}
namespace rec {
Pulse createPulseSink(){
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
    return {.m_name=name, .m_id=id};
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
    auto file = genFilename("ts");
    m_recVec.emplace_back(file);
    auto cmd = fmt::format(PIPELINE_WPE, m_params.uri, m_params.bitrate, file, m_pulse.m_name);
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
