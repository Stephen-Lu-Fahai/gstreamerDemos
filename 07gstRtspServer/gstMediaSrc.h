#ifndef GSTMEDIASRC_H
#define GSTMEDIASRC_H

#include <QObject>
#include <gst/gst.h>

class GstMediaSrc : public QObject {
    Q_OBJECT
public:
    GstMediaSrc(QString mediaCaps, int patten = 0);
    virtual ~GstMediaSrc();

    void saveBuffer(GstElement* sink, GstSample *sample);
    void setPipelineState(bool b);

    QString getMediaCap() { return mMediaCaps; }

signals:
    void signalSample(GstBuffer **audioBuf);

private:
    GstElement *mPipeline;
    GstElement *mAppSink;
    GstBuffer *mSrcBuf;

    QString mMediaCaps;
};


#endif // GSTMEDIASRC_H
