#include "appSrc.h"
#include <QDebug>

AppSrc::AppSrc()
{
    /* caps of app src must be same as input, otherwise appsrc doesn`t work */
    QString srcCaps = "caps=video/x-raw,format=I420,width=640,height=480,framerate=30/1";
    QString strPipeline = "appsrc name=appSrc " + srcCaps + " ! timeoverlay ! queue ! glimagesink";

    mPipeline = gst_parse_launch(strPipeline.toStdString().c_str(), nullptr);
    if (!mPipeline) {
        qDebug() << "failed to parse pipeline";
        return;
    }

    mAppSrc = gst_bin_get_by_name(GST_BIN(mPipeline), "appSrc");
    if (!mAppSrc) {
        qDebug() << "failed to parse appsrc from pipeline";
        return;
    }

    /* another way to set caps is as followed: */
//    g_object_set(appsrc1, "caps",
//        gst_caps_new_simple("video/x-raw",
//            "format", G_TYPE_STRING, "I420",
//            "width", G_TYPE_INT, 640,
//            "height", G_TYPE_INT, 360,
//            "framerate", GST_TYPE_FRACTION, 30, 1,
//            nullptr),
//      nullptr);

    /* set other properties if needs */
    g_object_set (G_OBJECT (mAppSrc),
        "format", GST_FORMAT_TIME,
        "is-live", TRUE,
        "do-timestamp", TRUE,
        NULL);


    mVideoTestSrc = new AppSink();
    connect(mVideoTestSrc, &AppSink::signalNewVideoSample,
            this, &AppSrc::onNewVideoSample);

    gst_element_set_state(mPipeline, GST_STATE_PLAYING);

    mInit = true;
}

AppSrc::~AppSrc()
{
    gst_object_unref(mPipeline);
    gst_object_unref(mAppSrc);
}


void AppSrc::onNewVideoSample(GstBuffer **buf)
{
    if (mInit) {
        GstFlowReturn ret;
        g_signal_emit_by_name (mAppSrc, "push-buffer", *buf, &ret);
    }
}


