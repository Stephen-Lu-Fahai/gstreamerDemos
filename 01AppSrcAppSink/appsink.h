#ifndef APPSINK_H
#define APPSINK_H
#include <gst/gst.h>
#include <QObject>

class AppSink : public QObject {
    Q_OBJECT
public:
    AppSink();
    ~AppSink();

    void SaveNewSample(GstSample *sample);

signals:
    void signalNewVideoSample(GstBuffer **buf);

private:
    GstElement *mPipeline = nullptr;
    GstElement *mAppSink = nullptr;
    GstBuffer *mVideoBuf;
};



#endif // APPSINK_H
