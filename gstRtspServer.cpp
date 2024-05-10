#include "gstRtspServer.h"
#include "gst/rtsp-server/rtsp-server.h"
#include <QDebug>

static GstRTSPServer *mServer;
static GstRTSPMountPoints *mMounts;
static GstRTSPMediaFactory *mFactory;

static void media_configure(GstRTSPMediaFactory *factory,
        GstRTSPMedia *media, GstRtspServer* obj)
{
    qDebug() << "gstRtsp media_configure";
    GstElement *pipeline = gst_rtsp_media_get_element(media);
    if (!pipeline) {
        qDebug() << "filed to get pipeline from rtsp server";
    }

    obj->parsePipeline(pipeline);
    gst_object_unref(pipeline);
}

GstRtspServer::GstRtspServer(string vedioCap, string audioCap)
{
    mServer = gst_rtsp_server_new();
    mMounts = gst_rtsp_server_get_mount_points(mServer);
    mFactory = gst_rtsp_media_factory_new();

    QString videoCaps = QString("caps=\"%1\"").arg(QString::fromStdString(vedioCap));
    QString videoUrl = "appsrc name=videoSrc " + videoCaps + " ! nvh264enc ! rtph264pay name=pay0 pt=96";

    QString audioCaps = QString("caps=\"%1\"").arg(QString::fromStdString(audioCap));
    QString audioUrl = " appsrc name=audioSrc is-live=0 " + audioCaps + " ! "
                        "audioconvert ! rtpL24pay name=pay1 pt=97";

//    QString stspUrl = "videotestsrc ! nvh264enc ! rtph264pay name=pay0 pt=96 "
//                      "audiotestsrc is-live=0 ! audioconvert ! audioconvert ! "
//                      "alawenc ! rtppcmapay name=pay1 pt=97";

    QString stspUrl = videoUrl + audioUrl;

    qDebug() << "rtsp: " << stspUrl;

    gst_rtsp_media_factory_set_launch (mFactory, QString("( %1 )").arg(stspUrl).toStdString().c_str());

    g_signal_connect(mFactory, "media-configure", (GCallback)media_configure, this);

    gst_rtsp_mount_points_add_factory (mMounts, "/proxinse", mFactory);
    qDebug() << "live url: rtsp://127.0.0.1:8554/proxinse";

    g_object_unref (mMounts);
    gst_rtsp_server_attach (mServer, NULL);
}

GstRtspServer::~GstRtspServer()
{

}

void GstRtspServer::parsePipeline(GstElement *pipeline)
{
    mVideoAppSrc = gst_bin_get_by_name_recurse_up(GST_BIN(pipeline), "videoSrc");
    if (!mVideoAppSrc) {
        qDebug() << "failed to get VideoSrc from rtsp pipeline";
        return;
    }

    mAudioAppSrc = gst_bin_get_by_name_recurse_up(GST_BIN(pipeline), "audioSrc");
    if (!mAudioAppSrc) {
        qDebug() << "failed to get audioSrc from rtsp pipeline";
        return;
    }

    gst_util_set_object_arg(G_OBJECT(mVideoAppSrc), "format", "time");
    gst_util_set_object_arg(G_OBJECT(mAudioAppSrc), "format", "time");

    mInited = true;
}


void GstRtspServer::sendVideoBuf(GstBuffer **buf)
{
    GstFlowReturn ret;

    if (mInited) {
        g_signal_emit_by_name(mVideoAppSrc, "push-buffer", *buf, &ret);
    }
}

void GstRtspServer::sendAudioBuf(GstBuffer **buf)
{
    GstFlowReturn ret;

    if (mInited) {
        g_signal_emit_by_name(mAudioAppSrc, "push-buffer", *buf, &ret);
    }
}




