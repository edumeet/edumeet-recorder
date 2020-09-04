#include "Service.h"
#include "Messages.h"
#include <Recorder/RecordManager.h>

using namespace web;
using namespace http;

CService::CService(const utility::string_t& address)
    : m_listener(address)
{
    m_listener.support(methods::GET, [this](const http_request& message) { handleGet(message); });
    m_listener.support(methods::POST, [this](const http_request& message) { handlePost(message); });
}

pplx::task<void> CService::open()
{
    return m_listener.open();
}

pplx::task<void> CService::close()
{
    return m_listener.close();
}

void CService::handleGet(http_request message)
{
    std::cout << message.to_string() << "\n";
    auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    if (paths.empty()) {
        message.reply(status_codes::BadRequest);
        return;
    }

    if (msg::STATUS == paths[0]) {
        if (auto status = rec::CRecordManager::getInstance().staus(); status.empty()) {
            message.reply(status_codes::OK);
        }
        else {
            auto response = json::value::parse(status.dump());
            message.reply(status_codes::OK, response);
        }
    }
    message.reply(status_codes::BadRequest);
}

void CService::handlePost(http_request message)
{
    std::cout << message.to_string() << "\n";
    auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    if (paths.empty()) {
        message.reply(status_codes::BadRequest);
        return;
    }

    if (msg::START == paths[0]) {
        auto json = message.extract_json().get();
        auto rtpPort(json.at("port").as_integer());
        auto rtcpPort(json.at("rtcpPort").as_integer());
        auto id = rec::CRecordManager::getInstance().startRecording(rtpPort, rtcpPort);
        if (id.has_value()) {
            auto response = json::value::object();
            response["status"] = json::value::string("started");
            message.reply(status_codes::OK, response);
        }
        else {
            auto error = json::value::object();
            error["error"] = json::value::string("Unable to start recording.");
            message.reply(status_codes::InternalError, error);
        }
    }

    if (msg::STOP == paths[0]) {
        rec::ID id(message.extract_json().get().at("id").as_integer());
        if (auto path = rec::CRecordManager::getInstance().stopRecording(id); path.has_value()) {
            auto response = json::value::object();
            response["status"] = json::value::string("stopped");
            response["path"] = json::value::string(path.value());
            message.reply(status_codes::OK, response);
        }
        else {
            auto error = json::value::object();
            error["error"] = json::value::string("Unable to stop recording.");
            message.reply(status_codes::InternalError, error);
        }
    }

    message.reply(status_codes::BadRequest);
}
