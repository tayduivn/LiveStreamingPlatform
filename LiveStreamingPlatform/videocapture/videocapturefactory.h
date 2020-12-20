#ifndef VIDEOCAPTUREFACTORY_H
#define VIDEOCAPTUREFACTORY_H

#include "opencvvideocapture.h"

enum VIDEOCAPTUREMETHOD
{
    OPENCV
};

class VideoCaptureFactory
{
public:
    static VideoCaptureAbstract *Get(VIDEOCAPTUREMETHOD method = OPENCV, unsigned index = 0);

protected:
    VideoCaptureFactory();
};

#endif // VIDEOCAPTUREFACTORY_H
