#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <catch2/catch.hpp>
#include <Recorder/Recorder.h>
#include <fmt/core.h>
#include <chrono>
#include <thread>

namespace {
auto constexpr RTP_PORT = 5000;
auto constexpr RTCP_PORT = 5001;
auto constexpr HOST = "127.0.0.1";
}

namespace tests {

struct Sender
{

    Sender()
    {
        auto cmd = fmt::format(
            "rtpbin name=rtpbin videotestsrc is-live=true ! vp8enc ! rtpvp8pay ! rtpbin.send_rtp_sink_0 "
            "rtpbin.send_rtp_src_0 ! udpsink port={0} host={2} "
            "rtpbin.send_rtcp_src_0 ! udpsink port={1} host={2} sync=false async=false",
            RTP_PORT, RTCP_PORT, HOST);
        m_pipe = gst_parse_launch(cmd.c_str(), nullptr);
        gst_element_set_state(m_pipe, GST_STATE_PLAYING);
    }
    ~Sender()
    {
        gst_element_set_state(m_pipe, GST_STATE_NULL);
    }

    GstElement* m_pipe;
};

SCENARIO("Recorder start/stop test")
{
    using namespace rec;
    CRecorder recorder("/tmp/abcDEFghi_rec_test");
    {
        auto ret = recorder.stop();
        CHECK(ret);
        auto start = std::chrono::steady_clock::now();
        CHECK(recorder.start(RTP_PORT, RTCP_PORT));
        auto stop = std::chrono::steady_clock::now();
        auto timeout = std::chrono::duration_cast<std::chrono::seconds>(stop - start).count();
        CHECK(9 < timeout);
        CHECK(15 > timeout);
        Sender s;
        CHECK(recorder.start(RTP_PORT, RTCP_PORT));
        CHECK(recorder.start(RTP_PORT, RTCP_PORT));
    }
    {
        auto ret = recorder.stop();
        CHECK(ret);
    }
    {
        auto ret = recorder.stop();
        CHECK(ret);
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

SCENARIO("Recorder record test")
{
    using namespace rec;
    namespace fs = std::filesystem;
    Sender sender;
    fs::path workdir = "/tmp/test_rec_ABC";
    fs::remove_all(workdir);
    auto counter = [&]() { return std::count_if(fs::directory_iterator(workdir), {}, [](fs::path p) { return fs::is_regular_file(p); }); };
    {
        CRecorder recorder(workdir);
        CHECK(0 == counter());
        auto path = recorder.getPath();
        CHECK(path.empty());
        auto started = recorder.start(RTP_PORT, RTCP_PORT);
        path = recorder.getPath();
        CHECK(path.empty());
        CHECK(started);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        auto stopped = recorder.stop();
        CHECK(stopped);
        path = recorder.getPath();
        CHECK(!path.empty());
        CHECK(path.parent_path() == workdir);
        CHECK(fs::exists(path));
        CHECK(fs::is_regular_file(path));
        CHECK(fs::file_size(path) > 0);
        started = recorder.start(RTP_PORT, RTCP_PORT);
        CHECK(started);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        CHECK(2 == counter());
    }
    {
        workdir = "/tmp/test_rec_DEF";
        fs::remove_all(workdir);
        CRecorder recorder(workdir);
        auto started = recorder.start(RTP_PORT, RTCP_PORT);
        CHECK(started);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        auto stopped = recorder.stop();
        CHECK(stopped);
        auto path = recorder.getPath();
        CHECK(!path.empty());
        CHECK(path.parent_path() == workdir);
        CHECK(fs::exists(path));
        CHECK(fs::is_regular_file(path));
        CHECK(fs::file_size(path) > 0);
        CHECK(1 == counter());
    }
}

} // namespace tests
