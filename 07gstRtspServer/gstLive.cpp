#include "gstLive.h"
#include <QDebug>

//#define Print_Buf_Len

GstLive::GstLive()
{
    QString videoCaps = QString("video/x-raw,format=%1,width=%2,height=%3,framerate=30/1")
                        .arg(mImgFormat).arg(mImgWidth).arg(mImgHeight);

    mGstVideoSrc = new GstMediaSrc(videoCaps, 18);
    connect(mGstVideoSrc, &GstMediaSrc::signalSample,
            this, &GstLive::onVideoSample);

    QString audioCaps = "audio/x-raw,format=S16LE,rate=44100,channels=2,layout=interleaved";

    mGstAudioSrc = new GstMediaSrc(audioCaps);
    connect(mGstAudioSrc, &GstMediaSrc::signalSample,
            this, &GstLive::onAudioSample);

    mGstRtspServer = new GstRtspServer(videoCaps.toStdString(), audioCaps.toStdString());

    mGstVideoSrc->setPipelineState(true);
    mGstAudioSrc->setPipelineState(true);
}

GstLive::~GstLive()
{
    delete mGstVideoSrc;
    delete mGstAudioSrc;
}


void GstLive::onVideoSample(GstBuffer **videoBuf)
{
#ifdef Print_Buf_Len
    GstMapInfo map;
    if (gst_buffer_map (*videoBuf, &map, GST_MAP_READ)) {
        qDebug() << "Video byteLen: " << (float)map.size / (float)(mImgWidth*mImgHeight);

        gst_buffer_unmap(*videoBuf, &map);
    }
#endif

    mGstRtspServer->sendVideoBuf(videoBuf);
}


void GstLive::onAudioSample(GstBuffer **audioBuf)
{
#ifdef Print_Buf_Len
    GstMapInfo map;
    if (gst_buffer_map (*videoBuf, &map, GST_MAP_READ)) {
        qDebug() << "Video byteLen: " << (float)map.size;
        gst_buffer_unmap(*videoBuf, &map);
    }

#endif

    mGstRtspServer->sendAudioBuf(audioBuf);
}
