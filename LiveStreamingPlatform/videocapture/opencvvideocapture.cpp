#include <iostream>
#include "opencvvideocapture.h"
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

struct OpencvVideoCapture::impl
{
    VideoCapture cam;
};

OpencvVideoCapture::OpencvVideoCapture() :
    pimpl(std::make_unique<impl>())
{

}

OpencvVideoCapture::~OpencvVideoCapture()
{

}

void OpencvVideoCapture::run()
{
    Mat frame;
    while (!isExit)
    {
        if (!pimpl->cam.read(frame))
        {
            msleep(1);
            continue;
        }
        if (frame.empty())
        {
            msleep(1);
            continue;
        }
        Data d((char*)frame.data, frame.cols*frame.rows*frame.elemSize(), GetCurTime());
        Push(d);
    }
}

bool OpencvVideoCapture::Init(int camIndex)
{
    pimpl->cam.open(camIndex);
    if (!pimpl->cam.isOpened())
    {
        cout << "cam open failed!" << endl;
        return false;
    }
    cout << camIndex << " cam open success" << endl;
    pimpl->cam.set(CV_CAP_PROP_FPS, 30);
    width = pimpl->cam.get(CAP_PROP_FRAME_WIDTH);
    height = pimpl->cam.get(CAP_PROP_FRAME_HEIGHT);
    fps = pimpl->cam.get(CAP_PROP_FPS);
    cout << "width" <<width<<endl;
    cout << "height" <<height<<endl;
    cout << "fps" <<fps<<endl;
    if (fps == 0) fps = 30;
    return true;
}

void OpencvVideoCapture::Stop()
{
    DataManager::Stop();
    if (pimpl->cam.isOpened())
    {
        pimpl->cam.release();
    }
}
