#ifndef GSTRTSPSERVER_H
#define GSTRTSPSERVER_H
#include <gst/gst.h>
#include <iostream>

using namespace std;

class GstRtspServer {
public:
    GstRtspServer(string vedioCap, string audioCap);
    ~GstRtspServer();

    void parsePipeline(GstElement *pipeline);

    void sendVideoBuf(GstBuffer **videoBuf);
    void sendAudioBuf(GstBuffer **videoBuf);

private:
    GstElement *mPipeline;
    GstElement *mVideoAppSrc;
    GstElement *mAudioAppSrc;

    bool mInited = false;
    GstBuffer buf;
};


#endif // GSTRTSPSERVER_H
