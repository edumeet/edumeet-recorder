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
        if (auto status = rec::CRecordManager::getInstance().status(); status.empty()) {
            message.reply(status_codes::OK);
            return;
        }
        else {
            auto response = json::value::parse(status.dump());
            message.reply(status_codes::OK, response);
            return;
        }
    }

    if (msg::GET_RECORDS == paths[0]) {
        rec::ID id(message.extract_json().get().at("id").as_integer());
        auto records = rec::CRecordManager::getInstance().getRecords(id);
        auto response = json::value::parse(records.dump());
        message.reply(status_codes::OK, response);
        return;
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
        auto id = rec::CRecordManager::getInstance().start(json.serialize().data());
        if (id.has_value()) {
            auto response = json::value::object();
            response["status"] = json::value::string("started");
            response["id"] = json::value::number(id.value());
            message.reply(status_codes::OK, response);
            return;
        }
        else {
            auto error = json::value::object();
            error["error"] = json::value::string("Unable to start recording. Missing or wrong parameters");
            message.reply(status_codes::InternalError, error);
            return;
        }
    }

    if (msg::STOP == paths[0]) {
        rec::ID id(message.extract_json().get().at("id").as_integer());
        if (rec::CRecordManager::getInstance().stop(id)) {
            auto response = json::value::object();
            response["status"] = json::value::string("stopped");
            message.reply(status_codes::OK, response);
            return;
        }
        else {
            auto error = json::value::object();
            error["error"] = json::value::string("Unable to stop recording.");
            message.reply(status_codes::InternalError, error);
            return;
        }
    }

    message.reply(status_codes::BadRequest);
}
