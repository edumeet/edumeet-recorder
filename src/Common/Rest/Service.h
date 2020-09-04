/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#ifndef RECORDER_SERVICE_SERVICE_H
#define RECORDER_SERVICE_SERVICE_H

#include <cpprest/http_listener.h>
#include <pplx/pplxtasks.h>
#include <string>

class CService {
public:
    explicit CService(const utility::string_t& address);
    pplx::task<void> open();
    pplx::task<void> close();

private:
    void handleGet(web::http::http_request message);
    void handlePost(web::http::http_request message);
    web::http::experimental::listener::http_listener m_listener;
};

#endif /* RECORDER_SERVICE_SERVICE_H */
