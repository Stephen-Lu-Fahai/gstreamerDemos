#include "audioSource.h"
#include <QDebug>

static GstFlowReturn new_sample(GstElement *sink, AudioSource *obj)
{
    GstSample *sample;
    g_signal_emit_by_name (sink, "pull-sample", &sample);

    if (sample) {
        obj->SaveNewSample(sample);
        gst_sample_unref(sample);
    }

    /* must return GST_FLOW_OK, or this function
        will be blocked*/
    return GST_FLOW_OK;
}

AudioSource::AudioSource()
{
    QString audioCaps = "audio/x-raw,rate=48000,channels=1,layout=interleaved,format=S16LE";

    QString strPipeline = "alsasrc device=hw:1,0 ! " + audioCaps + " ! appsink name=appSink";

    mPipeline = gst_parse_launch(strPipeline.toStdString().c_str(), nullptr);
    if (!mPipeline) {
        qDebug() << "failed to parse pipeline";
        return;
    }

    mAppSink = gst_bin_get_by_name(GST_BIN(mPipeline), "appSink");
    if (!mAppSink) {
        qDebug() << "failed to parse mAppSink from pipeline";
        return;
    }

    g_object_set (mAppSink, "emit-signals", TRUE, NULL);
    g_signal_connect (mAppSink, "new-sample", G_CALLBACK(new_sample), this);

    gst_element_set_state(mPipeline, GST_STATE_PLAYING);
    qDebug() << "AudioSource playing";
}

AudioSource::~AudioSource()
{
    gst_object_unref(mPipeline);
    gst_object_unref(mAppSink);
}

void AudioSource::SaveNewSample(GstSample *sample)
{
    mAudioBuf = gst_sample_get_buffer(sample);
    emit signalNewAudioSample(&mAudioBuf);
}

