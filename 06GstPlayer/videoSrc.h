#ifndef VIDEOSRC_H
#define VIDEOSRC_H

#include <QObject>
#include <gst/gst.h>

class GstVideoSrc : public QObject {
    Q_OBJECT
public:
    GstVideoSrc();
    ~GstVideoSrc();

    void saveVideoBuffer(GstElement* sink, GstSample *sample);
    void setVideoSrcState(bool state);

    QString getVideoCaps() { return mVideoCaps; }

signals:
    void signalVideoSample(GstBuffer **videoBuf);

private:
    GstElement *mPipeline;

    GstElement *mAppSink;
    GstBuffer *videoBuf;
    QString mVideoCaps;
};

#endif // VIDEOSRC_H
