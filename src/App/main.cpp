#include <Config/ConfigParser.h>
#include <Rest/Service.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/process/system.hpp>
#include <gst/gst.h>
#include <nlohmann/json.hpp>
#include <thread>

std::unique_ptr<CService> g_httpDealer;

void handler(const boost::system::error_code&, int)
{
    std::cout << "Closing service\n";
    g_httpDealer->close().wait();
}

void initService(const std::string& address)
{
    web::uri_builder uri(address);
    uri.append_path(U("api"));
    auto addr = uri.to_uri().to_string();
    g_httpDealer = std::make_unique<CService>(addr);
    g_httpDealer->open().wait();
    std::cout << "Listening for requests at: " << addr << "\n";
}

int main()
{
    gst_init(nullptr, nullptr);
    if (std::getenv("GST_DEBUG") == nullptr) {
        gst_debug_set_default_threshold(GST_LEVEL_WARNING);
    }
    CConfigParser::getInstance().init(boost::dll::program_location().parent_path().string() + "/config.json");

    std::string address = "http://localhost:34568";
    initService(address);

    boost::asio::io_service io_service;
    boost::asio::signal_set signals(io_service, SIGINT);
    signals.async_wait(handler);
    io_service.run();
    boost::process::system("pactl unload-module module-null-sink");
    return 0;
}
