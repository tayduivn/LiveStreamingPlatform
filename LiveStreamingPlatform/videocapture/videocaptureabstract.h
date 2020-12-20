#ifndef VIDEOCAPTUREABSTRACT_H
#define VIDEOCAPTUREABSTRACT_H

#include "data/datamanager.h"

class VideoCaptureAbstract : public DataManager
{
public:
    virtual bool Init(int camIndex = 0) = 0;
    virtual void Stop() = 0;

    int width = 0;
    int height = 0;
    int fps = 0;
};

#endif // VIDEOCAPTUREABSTRACT_H
