#include "ffmpegencoder.h"

extern "C"
{
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}
#include <iostream>
#include <future>

using namespace std;

#if defined WIN32 || defined _WIN32
#include <windows.h>
#endif
// Get number of cpu
static int GetCpuNum()
{
#if defined WIN32 || defined _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    return (int)sysinfo.dwNumberOfProcessors;
#elif defined __linux__
    return (int)sysconf(_SC_NPROCESSORS_ONLN);
#elif defined __APPLE__
    int numCPU = 0;
    int mib[4];
    size_t len = sizeof(numCPU);

    // set the mib for hw.ncpu
    mib[0] = CTL_HW;
    mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

    // get the number of CPUs from the system
    sysctl(mib, 2, &numCPU, &len, NULL, 0);

    if (numCPU < 1)
    {
        mib[1] = HW_NCPU;
        sysctl(mib, 2, &numCPU, &len, NULL, 0);

        if (numCPU < 1)
            numCPU = 1;
    }
    return (int)numCPU;
#else
    return 1;
#endif
}

struct FFmpegEncoder::impl
{
    // Context for size and format transformation
    SwsContext *swsContext = nullptr;
    // Context for resample
    SwrContext *swrContext = nullptr;
    // YUV output
    AVFrame *yuv = nullptr;
    // Resample output
    AVFrame *pcm = nullptr;
    // Video data of a frame
    AVPacket vpack = { 0 };
    // Audio data of a frame
    AVPacket apack = { 0 };
    int vpts = 0;
    int apts = 0;
    long long lastPts = -1;
};

FFmpegEncoder::FFmpegEncoder() :
    pimpl(std::make_unique<impl>())
{

}

FFmpegEncoder::~FFmpegEncoder()
{

}

void FFmpegEncoder::Close()
{
    if (pimpl->swsContext)
    {
        sws_freeContext(pimpl->swsContext);
        pimpl->swsContext = nullptr;
    }
    if (pimpl->swrContext)
    {
        swr_free(&pimpl->swrContext);
    }
    if (pimpl->yuv)
    {
        av_frame_free(&pimpl->yuv);
    }
    if (videoCodecContext)
    {
        avcodec_free_context(&videoCodecContext);
    }

    if (pimpl->pcm)
    {
        av_frame_free(&pimpl->pcm);
    }

    pimpl->vpts = 0;
    av_packet_unref(&pimpl->apack);
    pimpl->apts = 0;
    av_packet_unref(&pimpl->vpack);
}

bool FFmpegEncoder::InitScale()
{
    // Init swsContext
    pimpl->swsContext = sws_getCachedContext(pimpl->swsContext,
                                             inWidth, inHeight, AV_PIX_FMT_BGR24,
                                             outWidth, outHeight, AV_PIX_FMT_YUV420P,
                                             SWS_BICUBIC,
                                             0, 0, 0
                                             );
    if (!pimpl->swsContext)
    {
        cout << "sws_getCachedContext failed!";
        return false;
    }
    // Init struct of output data
    pimpl->yuv = av_frame_alloc();
    pimpl->yuv->format = AV_PIX_FMT_YUV420P;
    pimpl->yuv->width = inWidth;
    pimpl->yuv->height = inHeight;
    pimpl->yuv->pts = 0;

    // Allocate buffur for yuv
    int ret = av_frame_get_buffer(pimpl->yuv, 32);
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        throw exception(buf);
    }
    return true;
}

bool FFmpegEncoder::InitResample()
{
    pimpl->swrContext = nullptr;
    pimpl->swrContext = swr_alloc_set_opts(pimpl->swrContext,
                                           av_get_default_channel_layout(channels), (AVSampleFormat)outSampleFmt, sampleRate, //Output format
                                           av_get_default_channel_layout(channels), (AVSampleFormat)inSampleFmt, sampleRate, 0, 0); //Input format
    if (!pimpl->swrContext)
    {
        cout << "swr_alloc_set_opts failed!";
        return false;
    }
    int ret = swr_init(pimpl->swrContext);
    if (ret != 0)
    {
        char err[1024] = { 0 };
        av_strerror(ret, err, sizeof(err) - 1);
        cout << err << endl;
        return false;
    }

    // Audio ouput allocate
    pimpl->pcm = av_frame_alloc();
    pimpl->pcm->format = outSampleFmt;
    pimpl->pcm->channels = channels;
    pimpl->pcm->channel_layout = av_get_default_channel_layout(channels);
    pimpl->pcm->nb_samples = nbSample; // Sample number per frame in one channel
    ret = av_frame_get_buffer(pimpl->pcm, 0); // allocation for pcm
    if (ret != 0)
    {
        char err[1024] = { 0 };
        av_strerror(ret, err, sizeof(err) - 1);
        cout << err << endl;
        return false;
    }
    return true;
}

bool FFmpegEncoder::InitVideoCodec()
{
    if (!(videoCodecContext = CreateCodec(AV_CODEC_ID_H264)))
    {
        return false;
        cout << "InitVideoCodec error" << endl;
    }
    videoCodecContext->bit_rate = 50 * 1024 * 8;
    videoCodecContext->width = outWidth;
    videoCodecContext->height = outHeight;
    videoCodecContext->framerate = { fps,1 };

    videoCodecContext->gop_size = 50;
    videoCodecContext->max_b_frames = 0;
    videoCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    cout << "InitVideoCodec success" << endl;
    return OpenCodec(&videoCodecContext);
}

bool FFmpegEncoder::InitAudioCodec()
{
    if (!(audioCodecContext = CreateCodec(AV_CODEC_ID_AAC)))
    {
        return false;
    }
    audioCodecContext->bit_rate = 40000;
    audioCodecContext->sample_rate = sampleRate;
    audioCodecContext->sample_fmt = AV_SAMPLE_FMT_FLTP;
    audioCodecContext->channels = channels;
    audioCodecContext->channel_layout = av_get_default_channel_layout(channels);
    return OpenCodec(&audioCodecContext);
}

Data FFmpegEncoder::Resample(Data data)
{
    Data tmp;
    const uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
    indata[0] = (uint8_t *)data.data;
    int len = swr_convert(pimpl->swrContext, pimpl->pcm->data, pimpl->pcm->nb_samples, //Output parameter
                          indata, pimpl->pcm->nb_samples
                          );
    if (len <= 0)
    {
        return tmp;
    }
    pimpl->pcm->pts = data.pts;
    tmp.data = (char*)pimpl->pcm;
    tmp.size = pimpl->pcm->nb_samples*pimpl->pcm->channels * 2;
    tmp.pts = data.pts;
    return tmp;
}

Data FFmpegEncoder::RGBToYUV(Data rgb)
{
    Data tmp;
    tmp.pts = rgb.pts;
    //Input data
    uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
    //indata[0] bgrbgrbgr
    //plane indata[0] bbbbb indata[1]ggggg indata[2]rrrrr
    indata[0] = (uint8_t*)rgb.data;
    int insize[AV_NUM_DATA_POINTERS] = { 0 };

    insize[0] = inWidth * inPixSize;

    int h = sws_scale(pimpl->swsContext, indata, insize, 0, inHeight,
                      pimpl->yuv->data, pimpl->yuv->linesize);
    if (h <= 0)
    {
        return tmp;
    }
    pimpl->yuv->pts = rgb.pts;
    tmp.data = (char*)pimpl->yuv;
    int *p = pimpl->yuv->linesize;
    while ((*p))
    {
        tmp.size += (*p)*outHeight;
        p++;
    }
    return tmp;
}

Data FFmpegEncoder::EncodeVideo(Data frame)
{
    av_packet_unref(&pimpl->vpack);

    Data tmp;
    if (frame.size <= 0 || !frame.data)
        return tmp;
    AVFrame *p = (AVFrame *)frame.data;
    // h264 encoding
    int ret = avcodec_send_frame(videoCodecContext, p);
    if (ret != 0)
        return tmp;

    ret = avcodec_receive_packet(videoCodecContext, &pimpl->vpack);
    if (ret != 0 || pimpl->vpack.size <= 0)
        return tmp;
    tmp.data = (char*)(&pimpl->vpack);
    tmp.size = pimpl->vpack.size;
    tmp.pts = frame.pts;
    return tmp;
}

Data FFmpegEncoder::EncodeAudio(Data frame)
{
    Data tmp;
    if (frame.size <= 0 || !frame.data)
        return tmp;
    AVFrame *p = (AVFrame*)frame.data;
    if (pimpl->lastPts == p->pts)
    {
        p->pts += 1000;
    }
    pimpl->lastPts = p->pts;
    int ret = avcodec_send_frame(audioCodecContext, p);

    if (ret != 0)
        return tmp;
    av_packet_unref(&pimpl->apack);
    ret = avcodec_receive_packet(audioCodecContext, &pimpl->apack);
    if (ret != 0)
        return tmp;
    tmp.data = (char*)&pimpl->apack;
    tmp.size = pimpl->apack.size;
    tmp.pts = frame.pts;
    return tmp;
}

bool FFmpegEncoder::OpenCodec(AVCodecContext **c)
{
    std::future<int> fuRes = std::async(std::launch::async, avcodec_open2, *c, nullptr, nullptr);
    int ret = fuRes.get();
    if (ret != 0)
    {
        char err[1024] = { 0 };
        av_strerror(ret, err, sizeof(err) - 1);
        cout << err << endl;
        cout<< "error"<<endl;
        avcodec_free_context(c);
        return false;
    }
    cout << "avcodec_open2 success!" << endl;
    return true;
}

AVCodecContext *FFmpegEncoder::CreateCodec(AVCodecID cid)
{
    AVCodec *codec = avcodec_find_encoder(cid);
    if (!codec)
    {
        cout << "avcodec_find_encoder  failed!" << endl;
        return nullptr;
    }
    AVCodecContext* c = avcodec_alloc_context3(codec);
    if (!c)
    {
        cout << "avcodec_alloc_context3  failed!" << endl;
        return nullptr;
    }
    cout << "avcodec_alloc_context3 success!" << endl;

    c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    c->thread_count = GetCpuNum();
    c->time_base = { 1,1000000 };
    return c;
}
