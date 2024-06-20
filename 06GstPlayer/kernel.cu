#include <cuda.h>
#include <cuda_runtime.h>

#define NUM_THREAD_X 32
#define NUM_THREAD_Y 8

static int DivUp(int a, int b)
{
    return ((a % b) != 0) ? (a / b + 1) : (a / b);
}

__global__ void cuConvert_Rgb10A2_Rgba8(unsigned int *pSrc, unsigned char *pDst, int iWidth, int iHeight, int iSrcStride, int iDstStride)
{
    const int dx = blockIdx.x * blockDim.x + threadIdx.x;
    const int dy = blockIdx.y * blockDim.y + threadIdx.y;
    if (dx >= iWidth || dy >= iHeight) {
        return;
    }

    int idxSrc = dy * iSrcStride + dx;
    int idxDst = dy * iDstStride + dx * 4;

    uint r = pSrc[idxSrc] >> 22;
    uint g = pSrc[idxSrc] >> 12 & 1023;
    uint b = pSrc[idxSrc] >> 2 & 1023;

    b = b >> 2;
    g = g >> 2;
    r = r >> 2;

    //pDst[idxDst] = r << 24 | g << 16 | b << 8 | 255;
    //pDst[idxDst] = b << 24 | g << 16 | r << 8 | 255;
    pDst[idxDst]     = r;
    pDst[idxDst + 1] = g;
    pDst[idxDst + 2] = b;
    pDst[idxDst + 3] = 255;
}

extern "C"
void Convert_Rgb10A2_Rgba8(unsigned int *pSrc, unsigned char *pDst, int iWidth, int iHeight, int iSrcStride, int iDstStride)
{
    dim3 numThreads(NUM_THREAD_X, NUM_THREAD_Y, 1);
    dim3 numBlocks(DivUp(iWidth, numThreads.x), DivUp(iHeight, numThreads.y));
    cuConvert_Rgb10A2_Rgba8<<<numBlocks, numThreads>>>(pSrc, pDst, iWidth, iHeight, iSrcStride, iDstStride);
}

__global__ void cuRgba8_ResizeHeight(unsigned char *pSrc, unsigned char *pDst, int iWidth, int iHeight, int iStride, bool expand)
{
    const int dx = blockIdx.x * blockDim.x + threadIdx.x;
    const int dy = blockIdx.y * blockDim.y + threadIdx.y;

    if (dx >= iWidth || dy >= iHeight) {
        return;
    }

    int idxSrc = dy * iWidth + dx;
    int idxDst = dy * iWidth * 2 + dx;


    pDst[idxDst]          = pSrc[idxSrc];
    pDst[idxDst + iWidth] = pSrc[idxSrc];



//    int stepY = dy * iStride;
//    if (expand) {
//        stepY *= 2;
//    }
//    else {
//        stepY /= 2;
//    }

//    int idxSrc = dy * iStride + dx * 4;
//    int idxDst = stepY + dx * 4;

//    pDst[idxDst]     = pSrc[idxSrc];
//    pDst[idxDst + 1] = pSrc[idxSrc + 1];
//    pDst[idxDst + 2] = pSrc[idxSrc + 2];
//    pDst[idxDst + 3] = 255;
}

extern "C"
void rgba8_ResizeHeight(unsigned char *pSrc, unsigned char *pDst, int iWidth, int iHeight, int istride, bool expand)
{
    dim3 numThreads(NUM_THREAD_X, NUM_THREAD_Y, 1);
    dim3 numBlocks(DivUp(iWidth, numThreads.x), DivUp(iHeight, numThreads.y));

    cuRgba8_ResizeHeight<<<numBlocks, numThreads>>>(pSrc, pDst, iWidth, iHeight, istride, expand);
}



__global__ void cuRgba8_TopDownPack(unsigned char *pSrc1, unsigned char *pSrc2, unsigned char *pDst, int iWidth, int iHeight, int iStride)
{
    const int dx = blockIdx.x * blockDim.x + threadIdx.x;
    const int dy = blockIdx.y * blockDim.y + threadIdx.y;

    if (dx >= iWidth || dy >= iHeight) {
        return;
    }

    int idxSrc = dy * iStride + dx * 4;
    int idxTopDst = dy / 2 * iStride + dx * 4;
    int idxdownDst = (dy / 2 + iHeight / 2) * iStride + dx * 4;

    pDst[idxTopDst]     = pSrc1[idxSrc];
    pDst[idxTopDst + 1] = pSrc1[idxSrc + 1];
    pDst[idxTopDst + 2] = pSrc1[idxSrc + 2];
    pDst[idxTopDst + 3] = 255;

    pDst[idxdownDst]     = pSrc2[idxSrc];
    pDst[idxdownDst + 1] = pSrc2[idxSrc + 1];
    pDst[idxdownDst + 2] = pSrc2[idxSrc + 2];
    pDst[idxdownDst + 3] = 255;
}

extern "C"
void rgba8_TopDownPack(unsigned char *pSrc1, unsigned char *pSrc2, unsigned char *pDst, int iWidth, int iHeight, int istride)
{
    dim3 numThreads(NUM_THREAD_X, NUM_THREAD_Y, 1);
    dim3 numBlocks(DivUp(iWidth, numThreads.x), DivUp(iHeight, numThreads.y));

    cuRgba8_TopDownPack<<<numBlocks, numThreads>>>(pSrc1, pSrc2, pDst, iWidth, iHeight, istride);
}


__global__ void cuRgba8_LeftRightPack(unsigned char *pSrc1, unsigned char *pSrc2, unsigned char *pDst, int iWidth, int iHeight, int iStride)
{
    const int dx = blockIdx.x * blockDim.x + threadIdx.x;
    const int dy = blockIdx.y * blockDim.y + threadIdx.y;

    if (dx >= iWidth || dy >= iHeight) {
        return;
    }

        int idxSrc = dy * iStride + dx * 4;
        int idxLeftDst = dy * iStride + dx / 2 * 4;
        int idxRightDst = dy * iStride + (iWidth / 2 + dx / 2) * 4;

        pDst[idxLeftDst]     = pSrc1[idxSrc];
        pDst[idxLeftDst + 1] = pSrc1[idxSrc + 1];
        pDst[idxLeftDst + 2] = pSrc1[idxSrc + 2];
        pDst[idxLeftDst + 3] = 255;

        pDst[idxRightDst]     = pSrc2[idxSrc];
        pDst[idxRightDst + 1] = pSrc2[idxSrc + 1];
        pDst[idxRightDst + 2] = pSrc2[idxSrc + 2];
        pDst[idxRightDst + 3] = 255;
}

extern "C"
void rgba8_LeftRightPack(unsigned char *pSrc1, unsigned char *pSrc2, unsigned char *pDst, int iWidth, int iHeight, int istride)
{
    dim3 numThreads(NUM_THREAD_X, NUM_THREAD_Y, 1);
    dim3 numBlocks(DivUp(iWidth, numThreads.x), DivUp(iHeight, numThreads.y));

    cuRgba8_LeftRightPack<<<numBlocks, numThreads>>>(pSrc1, pSrc2, pDst, iWidth, iHeight, istride);
}


__global__ void cuRgba8_Blending(unsigned char *pSrc, unsigned char *pTopImageLayer, unsigned char *pDst, int iWidth, int iHeight, int iStride)
{
    const int dx = blockIdx.x * blockDim.x + threadIdx.x;
    const int dy = blockIdx.y * blockDim.y + threadIdx.y;

    if (dx >= iWidth || dy >= iHeight) {
        return;
    }

    int idx = dy * iStride + dx * 4;

    uint r = 0;
    uint g = 0;
    uint b = 0;

    if((pTopImageLayer[idx+3] == 0) ||
        (pTopImageLayer[idx] < 30 && pTopImageLayer[idx+1] < 30
         && pTopImageLayer[idx+2] < 30) ) {

        r = pSrc[idx];
        g = pSrc[idx + 1];
        b = pSrc[idx + 2];
    } else {
        r = pTopImageLayer[idx];
        g = pTopImageLayer[idx + 1];
        b = pTopImageLayer[idx + 2];
    }

    pDst[idx]     = r;
    pDst[idx + 1] = g;
    pDst[idx + 2] = b;
    pDst[idx + 3] = 255;
}

extern "C"
void rgba8_Blending(unsigned char *pSrc, unsigned char *pTopImageLayer, unsigned char *pDst, int iWidth, int iHeight, int istride)
{
    dim3 numThreads(NUM_THREAD_X, NUM_THREAD_Y, 1);
    dim3 numBlocks(DivUp(iWidth, numThreads.x), DivUp(iHeight, numThreads.y));

    cuRgba8_Blending<<<numBlocks, numThreads>>>(pSrc, pTopImageLayer, pDst, iWidth, iHeight, istride);
}


__global__ void cuYUYV422_to_rgba8(const unsigned char *yuvImage, unsigned char *rgbaImage, int width, int height)
{
    const int dx = blockIdx.x * blockDim.x + threadIdx.x;
    const int dy = blockIdx.y * blockDim.y + threadIdx.y;

    // 2 pixels per process unit
    if (dx < width/2 && dy < height) {
        int index = dy * width + dx * 2;
        int yuyvIdx = index * 2;
        int rgbaIdx = index * 4;

        // fetch
        unsigned char y0 = yuvImage[yuyvIdx];
        unsigned char u  = yuvImage[yuyvIdx + 1];
        unsigned char y1 = yuvImage[yuyvIdx + 2];
        unsigned char v  = yuvImage[yuyvIdx + 3];

        // YUYV to RGB
        int r0 = y0 + 1.402 * (v - 128);
        int g0 = y0 - 0.344136 * (u - 128) - 0.714136 * (v - 128);
        int b0 = y0 + 1.772 * (u - 128);

        int r1 = y1 + 1.402 * (v - 128);
        int g1 = y1 - 0.344136 * (u - 128) - 0.714136 * (v - 128);
        int b1 = y1 + 1.772 * (u - 128);

        // clamp
        r0 = min(max(r0, 0), 255);
        g0 = min(max(g0, 0), 255);
        b0 = min(max(b0, 0), 255);

        r1 = min(max(r1, 0), 255);
        g1 = min(max(g1, 0), 255);
        b1 = min(max(b1, 0), 255);

        // assign
        rgbaImage[rgbaIdx    ] = r0;
        rgbaImage[rgbaIdx + 1] = g0;
        rgbaImage[rgbaIdx + 2] = b0;
        rgbaImage[rgbaIdx + 3] = 255;

        rgbaImage[rgbaIdx + 4] = r1;
        rgbaImage[rgbaIdx + 5] = g1;
        rgbaImage[rgbaIdx + 6] = b1;
        rgbaImage[rgbaIdx + 7] = 255;
    }
}

extern "C"
void YUYV422_to_rgba8(const unsigned char *yuvImage, unsigned char *rgbaImage, int iWidth, int iHeight)
{
    dim3 blockSize(NUM_THREAD_X, NUM_THREAD_Y);
    dim3 gridSize(DivUp(iWidth, blockSize.x), DivUp(iHeight, blockSize.y));

    cuYUYV422_to_rgba8<<<gridSize, blockSize>>>(yuvImage, rgbaImage, iWidth, iHeight);
}

__global__ void cuYUYV422_to_rgba10A2(const unsigned char *yuvImage, unsigned int *rgbaImage, int width, int height)
{
    const int dx = blockIdx.x * blockDim.x + threadIdx.x;
    const int dy = blockIdx.y * blockDim.y + threadIdx.y;

    // 2 pixels per process unit
    if (dx < width/2 && dy < height) {
        int index = dy * width + dx * 2;
        int yuyvIdx = index * 2;
        int rgbaIdx = index;

        // fetch
        unsigned char y0 = yuvImage[yuyvIdx];
        unsigned char u  = yuvImage[yuyvIdx + 1];
        unsigned char y1 = yuvImage[yuyvIdx + 2];
        unsigned char v  = yuvImage[yuyvIdx + 3];

        // YUYV to RGB
        int r0 = y0 + 1.402 * (v - 128);
        int g0 = y0 - 0.344136 * (u - 128) - 0.714136 * (v - 128);
        int b0 = y0 + 1.772 * (u - 128);

        int r1 = y1 + 1.402 * (v - 128);
        int g1 = y1 - 0.344136 * (u - 128) - 0.714136 * (v - 128);
        int b1 = y1 + 1.772 * (u - 128);

        // clamp
        unsigned int ur0 = min(max(r0, 0), 255);
        unsigned int ug0 = min(max(g0, 0), 255);
        unsigned int ub0 = min(max(b0, 0), 255);

        unsigned int ur1 = min(max(r1, 0), 255);
        unsigned int ug1 = min(max(g1, 0), 255);
        unsigned int ub1 = min(max(b1, 0), 255);

        // assign
        rgbaImage[rgbaIdx    ] = ur0 << 24 | ug0 << 14 | ub0 << 4 | 3;
        rgbaImage[rgbaIdx + 1] = ur1 << 24 | ug1 << 14 | ub1 << 4 | 3;
    }
}

extern "C"
void YUYV422_to_rgba10A2(const unsigned char *yuvImage, unsigned int *rgbaImage, int iWidth, int iHeight)
{
    dim3 blockSize(NUM_THREAD_X, NUM_THREAD_Y);
    dim3 gridSize(DivUp(iWidth, blockSize.x), DivUp(iHeight, blockSize.y));

    cuYUYV422_to_rgba10A2<<<gridSize, blockSize>>>(yuvImage, rgbaImage, iWidth, iHeight);
}


__global__ void cuNV12_21_to_rgba8(const unsigned char *pSrc, int uvIndex, unsigned char *pDst, int iWidth, int iHeight, bool isNv12)
{
    const int dx = blockIdx.x * blockDim.x + threadIdx.x;
    const int dy = blockIdx.y * blockDim.y + threadIdx.y;

    if (dx < iWidth && dy < iHeight) {

        unsigned char y, u, v;
        int idxY, idxU, idxV;

        idxY = dy * iWidth + dx;
        y = pSrc[idxY];

        int idx1, idx2;
        int uvStep =  dy / 2 * iWidth + dx;

        if (dx % 2) {
            //column odd
            idx1 = uvStep - 1;
            idx2 = uvStep;
        }
        else {
            //column even
            idx1 = uvStep;
            idx2 = uvStep + 1;
        }

        idxU = isNv12 ? idx1 : idx2;
        idxV = isNv12 ? idx2 : idx1;

        u = pSrc[uvIndex + idxU];
        v = pSrc[uvIndex + idxV];

        int r = y + 1.402 * (v - 128);
        int g = y - 0.344136 * (u - 128) - 0.714136 * (v - 128);
        int b = y + 1.772 * (u - 128);

        r = min(max(r, 0), 255);
        g = min(max(g, 0), 255);
        b = min(max(b, 0), 255);

        int idxRgba = (dy * iWidth + dx) * 4;
        pDst[idxRgba] = r;
        pDst[idxRgba + 1] = g;
        pDst[idxRgba + 2] = b;
        pDst[idxRgba + 3] = 255;
    }
}

extern "C"
void nv12_21_to_rgba8(const unsigned char *pSrc, int uvIndex, unsigned char *pDst, int iWidth, int iHeight, bool isNv12)
{
    dim3 blockSize(NUM_THREAD_X, NUM_THREAD_Y);
    dim3 gridSize(DivUp(iWidth, blockSize.x), DivUp(iHeight, blockSize.y));

    cuNV12_21_to_rgba8<<<gridSize, blockSize>>>(pSrc, uvIndex, pDst, iWidth, iHeight, isNv12);
}

__global__ void cuI420_YV12_to_rgba8(const unsigned char *pSrc, unsigned char *pDst, int iWidth, int iHeight, bool isI420)
{
    const int dx = blockIdx.x * blockDim.x + threadIdx.x;
    const int dy = blockIdx.y * blockDim.y + threadIdx.y;

    if (dx < iWidth && dy < iHeight) {
        unsigned char y, u, v;

        int indexY = dy * iWidth + dx;
        y = pSrc[indexY];

        int idx1 = iWidth * iHeight;
        int idx2 = iWidth * iHeight * 5 / 4;

        int idxU = isI420 ? idx1 : idx2;
        int idxV = isI420 ? idx2 : idx1;

        /* core */
        int stepUV = (dy / 2 * iWidth + dx) / 2;

        u = pSrc[stepUV + idxU];
        v = pSrc[stepUV + idxV];

        int r = y + 1.402 * (v - 128);
        int g = y - 0.344136 * (u - 128) - 0.714136 * (v - 128);
        int b = y + 1.772 * (u - 128);

        r = min(max(r, 0), 255);
        g = min(max(g, 0), 255);
        b = min(max(b, 0), 255);

        int idxRgba = (dy * iWidth + dx) * 4;
        pDst[idxRgba] = r;
        pDst[idxRgba + 1] = g;
        pDst[idxRgba + 2] = b;
        pDst[idxRgba + 3] = 255;
    }
}

extern "C"
void i420_yv12_to_rgba8(const unsigned char *pSrc, unsigned char *pDst, int iWidth, int iHeight, bool isI420)
{
    dim3 blockSize(NUM_THREAD_X, NUM_THREAD_Y);
    dim3 gridSize(DivUp(iWidth, blockSize.x), DivUp(iHeight, blockSize.y));

    cuI420_YV12_to_rgba8<<<gridSize, blockSize>>>(pSrc, pDst, iWidth, iHeight, isI420);
}

