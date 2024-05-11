#include "audioSample.h"
#include <QDebug>

AudioSample::AudioSample()
{
    gst_init(NULL, NULL);
    QString strPipeline = "alsasrc device=hw:4,0 ! queue ! audioconvert ! autoaudiosink";
    GstElement * mPipeline = gst_parse_launch(strPipeline.toStdString().c_str(), nullptr);
    if (!mPipeline) {
        qDebug() << "failed to parse pipeline";
        return;
    }

    gst_element_set_state(mPipeline, GST_STATE_PLAYING);
}

AudioSample::~AudioSample()
{

}
