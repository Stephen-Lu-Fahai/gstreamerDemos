#include "display.h"
#include <QDebug>
#include <QTimer>
#include <cuda.h>
#include <cuda_runtime.h>
#include <npp.h>

extern "C" {
    void Convert_Rgb10A2_Rgba8(unsigned int *pSrc, unsigned char *pDst, int iWidth, int iHeight, int iSrcStride, int iDstStride);
    void rgba8_ResizeHeight(unsigned char *pSrc, unsigned char *pDst, int iWidth, int iHeight, int istride, bool expand);
    void rgba8_TopDownPack(unsigned char *pSrc1, unsigned char *pSrc2, unsigned char *pDst, int iWidth, int iHeight, int istride);
    void rgba8_LeftRightPack(unsigned char *pSrc1, unsigned char *pSrc2, unsigned char *pDst, int iWidth, int iHeight, int istride);
    void YUYV422_to_rgba8(const unsigned char *yuvImage, unsigned char *rgbaImage, int iWidth, int iHeight);
    void YUYV422_to_rgba10A2(const unsigned char *yuvImage, unsigned int *rgbaImage, int iWidth, int iHeight);

    void nv12_21_to_rgba8(const unsigned char *pSrc, int uvIndex, unsigned char *pDst, int iWidth, int iHeight, bool isNv12);
    void i420_yv12_to_rgba8(const unsigned char *pSrc, unsigned char *pDst, int iWidth, int iHeight, bool isI420);
}

GstDisplay::GstDisplay()
{
    mImgSize = mImgWidth * mImgHeight * 4;

    cudaMalloc(&mDevSrcBuf, mImgSize);
    cudaMalloc(&mDevDisplayBuf, mImgSize);
    mCpuBuf = (unsigned char*)malloc(mImgSize);

    mGstVideoSrc = new GstVideoSrc();
    connect(mGstVideoSrc, &GstVideoSrc::signalVideoSample,
            this, &GstDisplay::onVideoSample);

    mCpuBuf = (unsigned char*)malloc(mImgSize);

    QString appCap =  QString("caps=\"video/x-raw,format=%1,width=%2,height=%3,framerate=30/1\"")
                                .arg(mImgFormat).arg(mImgWidth).arg(mImgHeight);

    QString srcUrl = "appsrc format=time name=appsrc " + appCap + " ! glimagesink ";

    qDebug() << "displayUrl: " << srcUrl;

    mPipeline = gst_parse_launch(srcUrl.toStdString().c_str(), NULL);
    if (mPipeline == nullptr) {
        qDebug() << "failed to parse audio src pipeline";
        return;
    }

    mAppSrc = gst_bin_get_by_name(GST_BIN(mPipeline), "appsrc");

    mGstVideoSrc->setVideoSrcState(true);
    gst_element_set_state (mPipeline, GST_STATE_PLAYING);
}

GstDisplay::~GstDisplay()
{
    cudaFree(mDevSrcBuf);
    cudaFree(mDevDisplayBuf);

    free(mCpuBuf);
}

void GstDisplay::imageProcess()
{

    //rgba8_ResizeHeight(mDevSrcBuf, mDevDisplayBuf, 3840, 2160, 3840 * 4, true);

    nv12_21_to_rgba8(mDevSrcBuf, mImgWidth * mImgHeight, mDevDisplayBuf, mImgWidth, mImgHeight, false);
    //nv12_21_to_rgba8(mDevSrcBuf, mImgWidth * mImgHeight, mDevDisplayBuf, 3840, 2160, false);

    //i420_yv12_to_rgba8(mDevSrcBuf, mDevDisplayBuf, 3840, 2160, true);
}

void GstDisplay::onVideoSample(GstBuffer **videoBuf)
{
    GstFlowReturn ret;

    GstBuffer *sampleBuf = nullptr;
    GstMapInfo map;
    if (gst_buffer_map (*videoBuf, &map, GST_MAP_READ)) {
        //qDebug() << "byteLen: " << (float)map.size / (float)(mImgWidth*mImgHeight);

        cudaMemcpy(mDevSrcBuf, map.data, map.size, cudaMemcpyHostToDevice);
        imageProcess();

        cudaMemcpy(mCpuBuf, mDevDisplayBuf, mImgSize, cudaMemcpyDeviceToHost);
        sampleBuf = gst_buffer_new_wrapped(mCpuBuf, mImgSize);

        gst_buffer_unmap(*videoBuf, &map);
    }

    if (sampleBuf)
        g_signal_emit_by_name(mAppSrc, "push-buffer", sampleBuf, &ret);

}
