/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#include "Pipeline.h"
#include <iostream>
#include <stdexcept>

namespace {
auto constexpr TIMEOUT = 10 * GST_SECOND;
}

GstElement* init(const std::string& cmd)
{
    GError* error = nullptr;
    auto pipeline = gst_parse_launch(cmd.c_str(), &error);
    if (error) {
        g_error_free(error);
        throw std::runtime_error("Cannot init pipeline");
    }
    return pipeline;
}

CPipeline::CPipeline(std::string cmd)
    : m_cmd(std::move(cmd))
    , m_pipeline(init(m_cmd))
    , m_isRunning(false)
{
}

CPipeline::~CPipeline()
{
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
}

bool CPipeline::start()
{
    std::unique_lock lock(m_mtx);
    auto status = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (status == GST_STATE_CHANGE_ASYNC) {
        status = gst_element_get_state(m_pipeline, nullptr, nullptr, TIMEOUT);
    }
    if (status == GST_STATE_CHANGE_FAILURE) {
        return false;
    }
    m_isRunning = true;
    return true;
}

bool CPipeline::stop(const std::string& eosElement)
{
    std::unique_lock lock(m_mtx);
    if (eosElement.empty()) {
        gst_element_send_event(m_pipeline, gst_event_new_eos());
    }
    else {
        auto element = gst_bin_get_by_name(GST_BIN(m_pipeline), eosElement.c_str());
        gst_element_send_event(element, gst_event_new_eos());
        gst_object_unref(element);
    }
    auto bus = gst_element_get_bus(m_pipeline);
    auto msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
        static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    gst_object_unref(bus);
    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
        std::cout << "Error during record finalization\n";
    }
    gst_message_unref(msg);
    auto status = gst_element_set_state(m_pipeline, GST_STATE_NULL);
    if (status == GST_STATE_CHANGE_FAILURE) {
        std::cout << "Unable to stop recorder\n";
        return false;
    }
    else {
        m_isRunning = false;
        return true;
    }
}

bool CPipeline::isRunning()
{
    return m_isRunning;
}
