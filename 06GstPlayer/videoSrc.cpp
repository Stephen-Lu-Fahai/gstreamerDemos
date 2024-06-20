#include "videoSrc.h"
#include <QDebug>
#include <cuda.h>
#include <cuda_runtime.h>
#include <QTimer>

static GstFlowReturn onNewVideoSample(GstElement* sink_elm, GstVideoSrc* obj)
{
    GstSample *sample = NULL;
    GstFlowReturn ret;

    g_signal_emit_by_name(sink_elm, "pull-sample", &sample, &ret);

    if (sample) {
        obj->saveVideoBuffer(sink_elm, sample);
        gst_sample_unref(sample);
        return GST_FLOW_OK;
    }

    return GST_FLOW_ERROR;
}

GstVideoSrc::GstVideoSrc()
{
    if (!gst_is_initialized())
        gst_init(NULL, NULL);

    QString strPipeline = "videotestsrc ! video/x-raw,format=NV21,width=640,height=480,framerate=30/1 ! "
                          "appsink emit-signals=true name=appsink";

    qDebug() << "strPipeline: " << strPipeline;

    mPipeline = gst_parse_launch(strPipeline.toStdString().c_str(), NULL);
    if (mPipeline == nullptr) {
        qDebug() << "failed to parse audio src pipeline";
        return;
    }

    mAppSink = gst_bin_get_by_name(GST_BIN(mPipeline), "appsink");
    g_signal_connect (mAppSink, "new-sample",
      G_CALLBACK (onNewVideoSample), this);

    gst_element_set_state(mPipeline, GST_STATE_PLAYING);
}

GstVideoSrc::~GstVideoSrc()
{

}

void GstVideoSrc::setVideoSrcState(bool b)
{
    auto state = b ? GST_STATE_PLAYING : GST_STATE_NULL;
    gst_element_set_state (mPipeline, state);
}

void GstVideoSrc::saveVideoBuffer(GstElement* sink, GstSample *sample)
{
    videoBuf = gst_sample_get_buffer(sample);

    emit signalVideoSample(&videoBuf);
}

