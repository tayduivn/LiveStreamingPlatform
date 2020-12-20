#include "videocapturefactory.h"

VideoCaptureAbstract *VideoCaptureFactory::Get(VIDEOCAPTUREMETHOD method, unsigned index)
{
    static OpencvVideoCapture instances[255];
    return &instances[index];
}

VideoCaptureFactory::VideoCaptureFactory()
{

}
