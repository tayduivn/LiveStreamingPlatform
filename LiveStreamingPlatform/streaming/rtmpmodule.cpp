#include "rtmpmodule.h"
#include <iostream>
#include <string>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/time.h>
}

using namespace std;

struct RTMPModule::impl
{
    // Context for muxing
    AVFormatContext *muxingContext = nullptr;

    // Vidio codec
    const AVCodecContext *videoCodecContext = nullptr;

    // Audio codec
    const AVCodecContext *audioCodecContext = nullptr;

    // Video stream
    AVStream *videoStram = nullptr;

    // Audio stream
    AVStream *audioStream = nullptr;

    // Ouput url
    string url = "";
};

RTMPModule* RTMPModule::mInstance = nullptr;

RTMPModule::RTMPModule() : mImpl(make_unique<impl>())
{
    avformat_network_init();
    mInstance = nullptr;
}

RTMPModule* RTMPModule::Get() {
    if (mInstance == nullptr) {
        mInstance = new RTMPModule();
    }
    return mInstance;
}

bool RTMPModule::Init(const char *url)
{
    // Context for muxing
    int ret = avformat_alloc_output_context2(&mImpl->muxingContext, 0, "flv", url);
    mImpl->url = url;
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << buf;
        return false;
    }
    return true;
}

int RTMPModule::AddStream(const AVCodecContext *c)
{
    if (!c)return -1;

    // Add video stream
    AVStream *st = avformat_new_stream(mImpl->muxingContext, nullptr);
    if (!st)
    {
        cout << "avformat_new_stream failed" << endl;
        return -1;
    }
    st->codecpar->codec_tag = 0;
    // Copy parameter from codec
    avcodec_parameters_from_context(st->codecpar, c);
    av_dump_format(mImpl->muxingContext, 0, mImpl->url.c_str(), 1);

    if (c->codec_type == AVMEDIA_TYPE_VIDEO)
    {
        mImpl->videoCodecContext = c;
        mImpl->videoStram = st;
    }
    else if (c->codec_type == AVMEDIA_TYPE_AUDIO)
    {
        mImpl->audioCodecContext = c;
        mImpl->audioStream = st;
    }

    return st->index;
}

bool RTMPModule::SendHead()
{
    int ret = avio_open(&mImpl->muxingContext->pb, mImpl->url.c_str(), AVIO_FLAG_WRITE);
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << buf << endl;
        return false;
    }

    ret = avformat_write_header(mImpl->muxingContext, nullptr);
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << buf << endl;
        return false;
    }
    return true;
}

bool RTMPModule::SendFrame(Data data, int streamIndex)
{
    if (!data.data || data.size <= 0)
        return false;
    AVPacket *pack = (AVPacket *)data.data;
    pack->stream_index = streamIndex;
    AVRational stime;
    AVRational dtime;

    // Audio or video
    if (mImpl->videoStram && mImpl->videoCodecContext&& pack->stream_index == mImpl->videoStram->index)
    {
        stime = mImpl->videoCodecContext->time_base;
        dtime = mImpl->videoStram->time_base;
    }
    else if (mImpl->audioStream && mImpl->audioCodecContext&&pack->stream_index == mImpl->audioStream->index)
    {
        stime = mImpl->audioCodecContext->time_base;
        dtime = mImpl->audioStream->time_base;
    }
    else
    {
        return false;
    }

    //Upstreaming
    pack->pts = av_rescale_q(pack->pts, stime, dtime);
    pack->dts = av_rescale_q(pack->dts, stime, dtime);
    pack->duration = av_rescale_q(pack->duration, stime, dtime);
    int ret = av_interleaved_write_frame(mImpl->muxingContext, pack);
    if (ret != 0)
        return false;
    cout << "#" << flush;
    return true;
}

RTMPModule::~RTMPModule()
{
    if (mImpl->muxingContext)
    {
        avformat_close_input(&mImpl->muxingContext);
        mImpl->videoStram = nullptr;
    }
    mImpl->videoCodecContext = nullptr;
    mImpl->url = "";
}
