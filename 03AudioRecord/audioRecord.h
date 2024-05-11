#ifndef AUDIORECORD_H
#define AUDIORECORD_H
#include "audioSource.h"
#include <QObject>

class AudioRecord : public QObject {
    Q_OBJECT
public:
    AudioRecord();
    ~AudioRecord();

private slots:
    void onNewAudioSample(GstBuffer **buf);

private:
    GstElement *mPipeline = nullptr;
    GstElement *mAppSrc = nullptr;
    AudioSource *mAudioSource;
    bool mInit = false;
};

#endif // AUDIORECORD_H
