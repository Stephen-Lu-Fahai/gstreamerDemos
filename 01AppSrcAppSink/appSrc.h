#ifndef APPSRC_H
#define APPSRC_H
#include <gst/gst.h>
#include <QObject>
#include "appsink.h"

class AppSrc : public QObject {
    Q_OBJECT
public:
    AppSrc();
    ~AppSrc();

private slots:
    void onNewVideoSample(GstBuffer **buf);

private:
    GstElement *mPipeline = nullptr;
    GstElement *mAppSrc = nullptr;

    AppSink *mVideoTestSrc;
    bool mInit = false;
};


#endif // APPSRC_H
