#ifndef GSTLIVE_H
#define GSTLIVE_H

#include <QObject>
#include "gstMediaSrc.h"
#include "gstRtspServer.h"


class GstLive : public QObject {
    Q_OBJECT
public:
    GstLive();
    ~GstLive();

private slots:
    void onVideoSample(GstBuffer **videoBuf);
    void onAudioSample(GstBuffer **videoBuf);

private:
    GstMediaSrc *mGstVideoSrc;
    GstMediaSrc *mGstAudioSrc;

    GstRtspServer *mGstRtspServer;

    const int mImgWidth = 640;
    const int mImgHeight = 480;
    const QString mImgFormat = "RGBA";


};


#endif // GSTLIVE_H
