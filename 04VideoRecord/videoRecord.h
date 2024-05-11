#ifndef VIDEORECORD_H
#define VIDEORECORD_H

#include <gst/gst.h>
#include <QObject>
#include "videoSource.h"

class VideoRecord : public QObject {
    Q_OBJECT
public:
    VideoRecord();
    ~VideoRecord();

private slots:
    void onNewVideoSample(GstBuffer **buf);

private:
    GstElement *mPipeline = nullptr;
    GstElement *mAppSrc = nullptr;

    VideoSource *mVideoSrc;
    bool mInit = false;
};




#endif // VIDEORECORD_H
