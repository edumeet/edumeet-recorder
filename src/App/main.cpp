#include <Rest/Service.h>
#include <thread>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_service.hpp>
#include <nlohmann/json.hpp>
#include <Config/ConfigParser.h>
#include <boost/dll/runtime_symbol_info.hpp>
#include <gst/gst.h>

std::unique_ptr<CService> g_httpDealer;

void handler(const boost::system::error_code &, int) {
    std::cout << "Closing service\n";
    g_httpDealer->close().wait();
}

void initService(const std::string &address) {
    web::uri_builder uri(address);
    uri.append_path(U("api"));
    auto addr = uri.to_uri().to_string();
    g_httpDealer = std::make_unique<CService>(addr);
    g_httpDealer->open().wait();
    std::cout << "Listening for requests at: " << addr << "\n";
}

int main() {
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

    return 0;
}
