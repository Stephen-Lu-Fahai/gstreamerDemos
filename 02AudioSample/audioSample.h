#ifndef AUDIOSAMPLE_H
#define AUDIOSAMPLE_H
#include <gst/gst.h>
#include <QObject>

class AudioSample : public QObject {
    Q_OBJECT
public:
    AudioSample();
    ~AudioSample();
};



#endif // AUDIOSAMPLE_H
