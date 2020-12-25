#ifndef RTMPMODULE_H
#define RTMPMODULE_H

#include <memory>
#include "data/data.h"

struct AVCodecContext;
struct AVPacket;

class RTMPModule
{
public:
    // Singleton
    static RTMPModule *Get();

    // Init context for muxing
    bool Init(const char *url);

    // Add video stream or audio stream
    // return index if success, return -1 if fail
    int AddStream(const AVCodecContext *c);

    // Open rtmp io
    bool SendHead();

    // Upstreaming
    bool SendFrame(Data pkt, int streamIndex = 0);

    // Stop
    void Close();

private:
    static RTMPModule *mInstance;
    struct impl;
    std::unique_ptr<impl> mImpl;

    // Private constructor to prevent instancing.
    RTMPModule();
    ~RTMPModule();
};

#endif // RTMPMODULE_H
