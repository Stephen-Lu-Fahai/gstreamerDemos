#ifndef DISPLAY_H
#define DISPLAY_H

#include <QObject>
#include <gst/gst.h>
#include "videoSrc.h"

class GstDisplay : public QObject {
    Q_OBJECT
public:
    GstDisplay();
    ~GstDisplay();

public slots:
    void onVideoSample(GstBuffer **videoBuf);

private:
    void imageProcess();


private:
    GstElement *mPipeline;
    GstElement *mAppSrc = nullptr;

    unsigned char* mDevSrcBuf;
    unsigned char* mDevDisplayBuf;

    GstBuffer *mGstBuf;

    unsigned char* mCpuBuf;
    const int mImgWidth = 640;
    const int mImgHeight = 480;
    const QString mImgFormat = "RGBA";

    size_t mImgSize;

    GstVideoSrc *mGstVideoSrc;
};

#endif // DISPLAY_H
