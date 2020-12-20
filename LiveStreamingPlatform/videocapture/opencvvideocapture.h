#ifndef OPENCVVIDEOCAPTURE_H
#define OPENCVVIDEOCAPTURE_H

#include <memory>
#include "videocaptureabstract.h"

class OpencvVideoCapture : public VideoCaptureAbstract
{
public:
    OpencvVideoCapture();
    ~OpencvVideoCapture();
    void run();

    bool Init(int camIndex = 0) override;
    void Stop() override;

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

#endif // OPENCVVIDEOCAPTURE_H
