#ifndef GSTRECORDER_H
#define GSTRECORDER_H

#include <QThread>
#include "gstMediaSrc.h"

class GstRecorder : public QThread {
    Q_OBJECT
public:
    GstRecorder();
    ~GstRecorder();

private slots:
    void onVideoSample(int index, GstBuffer **videoBuf);
    void onAudioSample(GstBuffer **audioBuf);

private:
    void initMediaSource();
    void initRecordPipeline();

    void setVideoParam();

protected:
    void run() override;

private:
    GstMediaSrc *mVideoSrc1;
    GstMediaSrc *mVideoSrc2;
    GstMediaSrc *mAudioSrc;

    GstElement *mPipeline;
    GstElement *mAppVideoSrc1;
    GstElement *mAppVideoSrc2;
    GstElement *mAppAudioSrc;
    GstElement *mFileSink1;
    GstElement *mFileSink2;

    const int mImgWidth = 640;
    const int mImgHeight = 480;
    const QString mImgFormat = "RGBA";

    bool mInited = false;
    bool mRecoding = false;
    QString mVideoCaps;
    QString mAudioCaps;

    typedef enum {
        gstIdle = 0,
        gstSendBuffer,
        gstSendEos,
        gstStopWait,
        gstFinished,
    } State_t;
};


#endif // GSTRECORDER_H
