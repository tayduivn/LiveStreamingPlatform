#ifndef FFMPEGENCODER_H
#define FFMPEGENCODER_H

#include <memory>
#include "mediaencoderabstract.h"

struct AVCodecContext;
enum AVCodecID;

class FFmpegEncoder : public MediaEncoderAbstract
{
public:
    void Close();

    bool InitScale() override;
    bool InitResample() override;
    bool InitVideoCodec() override;
    bool InitAudioCodec() override;

    Data Resample(Data data) override;
    Data RGBToYUV(Data rgb) override;
    Data EncodeVideo(Data frame) override;
    Data EncodeAudio(Data frame) override;

    FFmpegEncoder();
    ~FFmpegEncoder();

private:
    bool OpenCodec(AVCodecContext **c);
    AVCodecContext *CreateCodec(AVCodecID cid);

    struct impl;
    std::unique_ptr<impl> pimpl;

};

#endif // FFMPEGENCODER_H
