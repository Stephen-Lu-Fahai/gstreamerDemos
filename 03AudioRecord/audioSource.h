#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H
#include <gst/gst.h>
#include <QObject>

class AudioSource : public QObject {
    Q_OBJECT
public:
    AudioSource();
    ~AudioSource();

    void SaveNewSample(GstSample *sample);

signals:
    void signalNewAudioSample(GstBuffer **buf);

private:
    GstElement *mPipeline = nullptr;
    GstElement *mAppSink = nullptr;
    GstBuffer *mAudioBuf;
};



#endif // AUDIOSOURCE_H
