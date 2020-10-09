#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <Config/ConfigParser.h>
#include <boost/dll/runtime_symbol_info.hpp>
#include <gst/gst.h>
#include <atomic>

int main(int argc, char* argv[])
{
    CConfigParser::getInstance().init(boost::dll::program_location().parent_path().string() + "/config.json");
    gst_init(nullptr, nullptr);
    if (std::getenv("GST_DEBUG") == nullptr) {
        gst_debug_set_default_threshold(GST_LEVEL_WARNING);
    }
    Catch::Session session;
    bool ro = false;
    using namespace Catch::clara;
    auto cli = session.cli() | Opt(ro)["--random-output"]("set random output");

    session.cli(cli);
    auto CATCH2_ARGS_C = std::getenv("CATCH2_ARGS");
    int returnCode = -1;
    if (CATCH2_ARGS_C != nullptr) {
        std::vector<const char*> new_argv(argv, argv + argc);
        char* token = strtok(CATCH2_ARGS_C, " ");
        while (token != nullptr) {
            new_argv.push_back(token);
            token = strtok(nullptr, " ");
        }
        returnCode = session.applyCommandLine(new_argv.size(), new_argv.data());
    }
    else {
        returnCode = session.applyCommandLine(argc, argv);
    }
    if (returnCode != 0) {
        return returnCode;
    }

    std::atomic<int> outCount = 0;
    if (static_cast<int>(ro) > 0) {
        session.configData().outputFilename += std::to_string(++outCount) + ".xml";
        session.configData().reporterName = "junit";
    }

    return session.run();
}