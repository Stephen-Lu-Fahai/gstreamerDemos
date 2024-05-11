#include "videoRecord.h"
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QProcess>
#include <gst/app/gstappsrc.h>

VideoRecord::VideoRecord()
{
    QString output = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss") + ".mp4";

    /* caps of app src must be same as input, otherwise appsrc doesn`t work */
    QString srcCaps = "caps=video/x-raw,format=I420,width=640,height=480,framerate=30/1";
    QString strPipeline = "appsrc name=appSrc " + srcCaps + " ! "
                          "nvh264enc ! h264parse ! avimux ! filesink location=" + output;

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

    /* set other properties if needs */
    g_object_set (G_OBJECT (mAppSrc),
        "stream-type", GST_APP_STREAM_TYPE_STREAM,
        "format", GST_FORMAT_TIME,
        "is-live", TRUE,
        "do-timestamp", TRUE,
        NULL);

    mVideoSrc = new VideoSource();
    connect(mVideoSrc, &VideoSource::signalNewVideoSample,
            this, &VideoRecord::onNewVideoSample);

    gst_element_set_state(mPipeline, GST_STATE_PLAYING);

    mInit = true;
}

VideoRecord::~VideoRecord()
{
    gst_object_unref(mPipeline);
    gst_object_unref(mAppSrc);
}


void VideoRecord::onNewVideoSample(GstBuffer **buf)
{
    if (mInit) {
        GstFlowReturn ret;
        qDebug() << "onNewVideoSample";
        g_signal_emit_by_name (mAppSrc, "push-buffer", *buf, &ret);
    }
}



