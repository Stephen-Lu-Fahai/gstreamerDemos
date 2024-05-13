#include "gstRecorder.h"
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QDateTime>


GstRecorder::GstRecorder()
{
    initMediaSource();
    initRecordPipeline();
}

GstRecorder::~GstRecorder()
{
    delete mVideoSrc1;
    delete mVideoSrc2;
    delete mAudioSrc;
}

void GstRecorder::initMediaSource()
{
    mVideoCaps = QString("video/x-raw,format=%1,width=%2,height=%3,framerate=30/1")
                        .arg(mImgFormat).arg(mImgWidth).arg(mImgHeight);

    mVideoSrc1 = new GstMediaSrc(mVideoCaps, 18);
    connect(mVideoSrc1, &GstMediaSrc::signalSample, this,
            [this](GstBuffer **videoBuf) ->void {
                onVideoSample(1, videoBuf);
    });

    mVideoSrc2 = new GstMediaSrc(mVideoCaps);
    connect(mVideoSrc2, &GstMediaSrc::signalSample, this,
            [this](GstBuffer **videoBuf) ->void {
                onVideoSample(2, videoBuf);
    });

    mAudioCaps = "audio/x-raw,format=S16LE,rate=44100,channels=2,layout=interleaved";

    mAudioSrc = new GstMediaSrc(mAudioCaps);
    connect(mAudioSrc, &GstMediaSrc::signalSample,
            this, &GstRecorder::onAudioSample);
}

void GstRecorder::initRecordPipeline()
{
    QString strVideoSrc = " appsrc format=time is-live=true do-timestamp=true caps=\"" + mVideoCaps + "\"";
    QString strVideoSrc1 = strVideoSrc + " name=videoSrc1 ! ";
    QString strVideoSrc2 = strVideoSrc + " name=videoSrc2 ! ";

    QString strTags = "taginject tags=";
    QString strVideoMetaData1 = strTags + "\"comment=video1\" ! ";
    QString strVideoMetaData2 = strTags + "\"comment=video2\" ! ";

    QString strEncoder = "nvh264enc ! h264parse ! ";

    QString strMux1Name = "mux1";
    QString strMux2Name = "mux2";
    QString strMux1 = "mp4mux name=" + strMux1Name;
    QString strMux2 = "mp4mux name=" + strMux2Name;

    QString strAudioTee = "audioTee";

    QString strAudioSrc1 = " appsrc format=time is-live=true do-timestamp=true name=audioSrc caps=\"" + mAudioCaps +
               "\" ! volume volume=1.0 name=audioVolume ! audioresample ! "
               "audio/x-raw,rate=48000 ! queue ! voaacenc bitrate=128000 ! "
               "audio/mpeg ! aacparse ! audio/mpeg,mpegversion=4 !"
               " tee name=" + strAudioTee + " ! queue ! mux1. ";

    /* both video use the same audio input */
    QString strAudioSrc2 = strAudioTee + ". ! queue ! " + strMux2Name + ". ";

    QString strFileSink1 = strMux1Name + ". ! queue ! filesink name=filesink1";
    QString strFileSink2 = strMux2Name + ". ! queue ! filesink name=filesink2";

    QString strRecord1 = strVideoSrc1 + strVideoMetaData1 + strEncoder +
                         strMux1 + strAudioSrc1 + strFileSink1;

    QString strRecord2 = strVideoSrc2 + strVideoMetaData2 +  strEncoder +
                         strMux2 + " " + strAudioSrc2 + strFileSink2;

    QString strPipeline = strRecord1 + strRecord2;

    qDebug() << "record pipeline: " << strPipeline;

    mPipeline = gst_parse_launch(strPipeline.toStdString().c_str(), NULL);
    if (mPipeline == nullptr) {
        qDebug() << "failed to parse src pipeline";
        return;
    }

    mAppVideoSrc1 = gst_bin_get_by_name(GST_BIN(mPipeline), "videoSrc1");
    mAppVideoSrc2 = gst_bin_get_by_name(GST_BIN(mPipeline), "videoSrc2");
    mAppAudioSrc = gst_bin_get_by_name(GST_BIN(mPipeline), "audioSrc");

    mFileSink1 = gst_bin_get_by_name(GST_BIN(mPipeline), "filesink1");
    mFileSink2 = gst_bin_get_by_name(GST_BIN(mPipeline), "filesink2");

    if (!mAppVideoSrc1 || !mAppVideoSrc2 ||
        !mAppAudioSrc || !mFileSink1 || !mFileSink2) {
        qDebug() << "failed to parse elements from pipeline";
        return;
    }

    mInited = true;
}

void GstRecorder::onVideoSample(int index, GstBuffer **videoBuf)
{
    if (mRecoding) {
        GstFlowReturn ret;
        auto appsrc = (index == 1) ? mAppVideoSrc1 : mAppVideoSrc2;
        g_signal_emit_by_name(appsrc, "push-buffer", *videoBuf, &ret);
    }

    /* another way to push buffer:
        gst_app_src_push_buffer(appsrc, *videoBuf);
    */
}

void GstRecorder::onAudioSample(GstBuffer **audioBuf)
{

    if (mRecoding) {
        //qDebug() << "onAudioSample";
        GstFlowReturn ret;
        g_signal_emit_by_name(mAppAudioSrc, "push-buffer", *audioBuf, &ret);
    }

    /* another way to push buffer:
        gst_app_src_push_buffer(mAudioSrc, *audioBuf);
    */
}

void GstRecorder::setVideoParam()
{
    mVideoSrc1->setPipelineState(true);
    mVideoSrc2->setPipelineState(true);
    mAudioSrc->setPipelineState(true);

    QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
    QString video1 = "video1-" + curTime + ".mp4";
    QString video2 = "video2-" + curTime + ".mp4";

    g_object_set(mFileSink1, "location", video1.toStdString().c_str(), nullptr);
    g_object_set(mFileSink2, "location", video2.toStdString().c_str(), nullptr);

    gst_element_set_state(mPipeline, GST_STATE_PLAYING);
    mRecoding = true;
}

void GstRecorder::run()
{
    State_t state = gstIdle;
    int cnt = 0;

    while (1) {
        switch (state) {
        case gstIdle:
            setVideoParam();
            state = gstSendBuffer;
            break;

        case gstSendBuffer:
            if (cnt++ > 30 * 10) {
                state = gstSendEos;
            }

            break;
        case gstSendEos:
            mRecoding = false;
            gst_element_send_event(mAppVideoSrc1, gst_event_new_eos());
            gst_element_send_event(mAppVideoSrc2, gst_event_new_eos());
            gst_element_send_event(mAppAudioSrc, gst_event_new_eos());

            /* another way to push buffer:
                gst_app_src_end_of_stream(mAppAudioSrc);
            */
            state = gstStopWait;
            break;
        case gstStopWait:
            gst_element_set_state(mPipeline, GST_STATE_PAUSED);
            gst_element_set_state(mPipeline, GST_STATE_NULL);
            state = gstFinished;
            qDebug() << "record finished";
            break;
        }

        msleep(33);
    }
}


