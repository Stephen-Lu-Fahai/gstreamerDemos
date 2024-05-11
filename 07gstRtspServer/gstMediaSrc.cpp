#include "gstMediaSrc.h"
#include <QDebug>
#include <QTimer>

static GstFlowReturn onNewSample(GstElement* sink_elm, GstMediaSrc* obj)
{
    GstSample *sample = NULL;
    GstFlowReturn ret;

    g_signal_emit_by_name(sink_elm, "pull-sample", &sample, &ret);

    if (sample) {
        obj->saveBuffer(sink_elm, sample);
    }
    gst_sample_unref(sample);

    return GST_FLOW_OK;
}

GstMediaSrc::GstMediaSrc(QString mediaCaps, int patten)
    : mMediaCaps(mediaCaps)
{
    if (!gst_is_initialized())
        gst_init(NULL, NULL);

    QString strSrc;
    QString strPattern;
    if (mediaCaps.contains("video/x-raw")) {
        strSrc = "videotestsrc ";
        strPattern = QString("pattern=%1").arg(patten);
    }
    else if (mediaCaps.contains("audio/x-raw")) {
        strSrc = "audiotestsrc ";
        strPattern = QString("wave=%1").arg(patten);
    }
    else {
        qDebug() << "unsupported media!";
        return;
    }

    QString strUrl = strSrc + strPattern + " ! " + mediaCaps +
                        " ! appsink emit-signals=true name=appsink";

    qDebug() << "strUrl: " << strUrl;

    mPipeline = gst_parse_launch(strUrl.toStdString().c_str(), NULL);
    if (mPipeline == nullptr) {
        qDebug() << "failed to parse audio src pipeline";
        return;
    }

    mAppSink = gst_bin_get_by_name(GST_BIN(mPipeline), "appsink");

    g_signal_connect (mAppSink, "new-sample",
            G_CALLBACK (onNewSample), this);
}

GstMediaSrc::~GstMediaSrc()
{
    g_object_unref(mPipeline);
}

void GstMediaSrc::setPipelineState(bool b)
{
    auto state = b ? GST_STATE_PLAYING : GST_STATE_NULL;
    gst_element_set_state (mPipeline, state);
}

void GstMediaSrc::saveBuffer(GstElement* sink, GstSample *sample)
{
    mSrcBuf = gst_sample_get_buffer(sample);
    emit signalSample(&mSrcBuf);
}

