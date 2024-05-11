#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <gst/gst.h>
#include <QObject>

class VideoSource : public QObject {
    Q_OBJECT
public:
    VideoSource();
    ~VideoSource();

    void SaveNewSample(GstSample *sample);

signals:
    void signalNewVideoSample(GstBuffer **buf);

private:
    GstElement *mPipeline = nullptr;
    GstElement *mAppSink = nullptr;
    GstBuffer *mVideoBuf;
};



#endif // VIDEOSOURCE_H
