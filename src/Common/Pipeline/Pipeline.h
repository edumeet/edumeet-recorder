/* Copyright (C) 2020
 *
 * Authors: Wojciech Kapsa
*/

#ifndef RECORDER_SERVICE_PIPELINE_H
#define RECORDER_SERVICE_PIPELINE_H

#include <gst/gst.h>
#include <atomic>
#include <mutex>
#include <string>

class CPipeline
{
public:
    explicit CPipeline(std::string cmd);
    bool start();
    bool stop(const std::string& eosElement);
    bool isRunning();
    template <typename... Args>
    void set(const std::string& element, Args&&... a);
    virtual ~CPipeline();

private:
    std::string m_cmd;
    GstElement* m_pipeline;
    std::atomic<bool> m_isRunning;
    std::mutex m_mtx;
};

template <typename... Args>
void CPipeline::set(const std::string& element, Args&&... a)
{
    if (!m_pipeline) {
        return;
    }
    auto el = gst_bin_get_by_name(GST_BIN(m_pipeline), element.c_str());
    if (el) {
        g_object_set(el, a..., nullptr);
    }
    gst_object_unref(el);
}

#endif //RECORDER_SERVICE_PIPELINE_H
