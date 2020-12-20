#include "mediaencoderfactory.h"

MediaEncoderAbstract *MediaEncoderFactory::Get(ENCODEMETHOD method, unsigned index)
{
    static FFmpegEncoder instance[255];
    return &instance[index];
}

MediaEncoderFactory::MediaEncoderFactory()
{

}
