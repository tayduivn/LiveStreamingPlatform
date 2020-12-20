#ifndef MEDIAENCODERFACTORY_H
#define MEDIAENCODERFACTORY_H

#include "ffmpegencoder.h"

enum ENCODEMETHOD
{
    FFMPEG
};

class MediaEncoderFactory
{
public:
    static MediaEncoderAbstract *Get(ENCODEMETHOD method = FFMPEG, unsigned index = 0);

protected:
    MediaEncoderFactory();
};

#endif // MEDIAENCODERFACTORY_H
