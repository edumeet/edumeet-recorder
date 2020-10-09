#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <catch2/catch.hpp>
#include <Recorder/UriRecorder.h>
#include <gst/pbutils/gstdiscoverer.h>
#include <chrono>
#include <thread>

namespace tests {

SCENARIO("Recorder html start/stop test")
{
    using namespace rec;

    nlohmann::json params_json = R"({
    "bitrate": 3000,
    "uri": "https://google.com"
    })"_json;

    auto params = params_json.get<CHtmlRecorder::Params>();
    CHtmlRecorder recorder("/tmp/ABCdefGHI_rec_test", params);
    CHECK(recorder.stop());
    CHECK(recorder.start());

    std::this_thread::sleep_for(std::chrono::seconds(7));
    CHECK(recorder.stop());
    auto record = recorder.getRecords()[0];

    auto discoverer = gst_discoverer_new(5 * GST_SECOND, nullptr);
    auto info = gst_discoverer_discover_uri(discoverer, ("file://" + record.string()).c_str(), nullptr);
    CHECK(4000 < (gst_discoverer_info_get_duration(info) / GST_MSECOND));
    CHECK((gst_discoverer_info_get_duration(info) / GST_MSECOND) < 10000);
    auto audio_list = gst_discoverer_info_get_audio_streams(info);
    CHECK(g_list_length(audio_list) == 1);
    auto video_list = gst_discoverer_info_get_video_streams(info);
    CHECK(g_list_length(video_list) == 1);
    gst_discoverer_info_unref(info);
}

} // namespace tests
