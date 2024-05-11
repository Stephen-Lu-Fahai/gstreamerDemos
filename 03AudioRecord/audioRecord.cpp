#include "audioRecord.h"
#include <QDebug>

AudioRecord::AudioRecord()
{
    QString srcCaps = "caps=audio/x-raw,rate=48000,channels=1,layout=interleaved,format=S16LE";
    QString strPipeline = "appsrc name=appSrc " + srcCaps + " ! wavenc ! "
                                "queue ! filesink location=out.wav";

    mPipeline = gst_parse_launch(strPipeline.toStdString().c_str(), nullptr);
    if (!mPipeline) {
        qDebug() << "failed to parse pipeline";
        return;
    }

    mAppSrc = gst_bin_get_by_name(GST_BIN(mPipeline), "appSrc");
    if (!mAppSrc) {
        qDebug() << "failed to parse appsrc from pipeline";
        return;
    }

    mAudioSource = new AudioSource();
    connect(mAudioSource, &AudioSource::signalNewAudioSample,
            this, &AudioRecord::onNewAudioSample);

    gst_element_set_state(mPipeline, GST_STATE_PLAYING);

    mInit = true;
}

AudioRecord::~AudioRecord()
{
    delete mAudioSource;
    gst_object_unref(mPipeline);
    gst_object_unref(mAppSrc);
}

void AudioRecord::onNewAudioSample(GstBuffer **buf)
{
    if (mInit) {
        GstFlowReturn ret;
        g_signal_emit_by_name(mAppSrc, "push-buffer", *buf, &ret);
    }
}
