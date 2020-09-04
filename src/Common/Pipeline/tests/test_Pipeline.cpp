#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <catch2/catch.hpp>
#include <Pipeline/Pipeline.h>
#include <thread>

namespace tests {
SCENARIO("Pipeline test")
{
    CPipeline pipe("videotestsrc ! fakesink");
    CHECK(pipe.start());
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(3s);
}

} // namespace tests
