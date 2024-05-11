#include "appsink.h"

static GstFlowReturn new_sample (GstElement *sink, AppSink *obj)
{
    GstSample *sample;
    g_signal_emit_by_name (sink, "pull-sample", &sample);

    if (sample) {
        obj->SaveNewSample(sample);
        gst_sample_unref(sample);
    }

    /* must return GST_FLOW_OK, or this function
        will be blocked*/
    return GST_FLOW_OK;
}

AppSink::AppSink()
{
    QString videoCaps = "video/x-raw,format=I420,width=640,height=480,framerate=30/1";
    QString strPipeline = "videotestsrc ! videoconvert ! " + videoCaps + " ! appsink name=videoSink";

    mPipeline = gst_parse_launch(strPipeline.toStdString().c_str(), nullptr);
    mAppSink = gst_bin_get_by_name(GST_BIN(mPipeline), "videoSink");

    g_object_set (mAppSink, "emit-signals", TRUE, NULL);
    g_signal_connect (mAppSink, "new-sample", G_CALLBACK(new_sample), this);

    gst_element_set_state(mPipeline, GST_STATE_PLAYING);
}

AppSink::~AppSink()
{
    gst_object_unref(mPipeline);
    gst_object_unref(mAppSink);
}

void AppSink::SaveNewSample(GstSample *sample)
{
    mVideoBuf = gst_sample_get_buffer(sample);
    emit signalNewVideoSample(&mVideoBuf);
}
