#include <iostream>
#include "videocapture/videocapturefactory.h"
#include "audiorecord/audiorecorderfactory.h"
#include "codec/mediaencoderfactory.h"
#include "streaming/rtmpmodule.h"
#include "controller.h"

using namespace std;

Controller *Controller::mInstance = nullptr;

Controller *Controller::Get()
{
    if (mInstance == nullptr) {
        mInstance = new Controller();
    }
    return mInstance;
}

void Controller::run()
{
    long long beginTime = GetCurTime();
    while (!isExit)
    {
        // Get a frame data
        Data ad = AudioRecorderFactory::Get()->Pop();
        Data vd = VideoCaptureFactory::Get()->Pop();
        if (ad.size<=0 && vd.size<=0)
        {
            msleep(1);
            continue;
        }
        //audio processing
        if (ad.size > 0)
        {
            ad.pts = ad.pts - beginTime;

            // Resample
            Data pcm = MediaEncoderFactory::Get()->Resample(ad);
            ad.Release();
            Data pkt = MediaEncoderFactory::Get()->EncodeAudio(pcm);
            if (pkt.size > 0)
            {
                ////Upstreaming
                if (RTMPModule::Get()->SendFrame(pkt,audioIdx))
                {
                    cout << "#" << flush;
                }
            }
        }
        //Video processing
        if (vd.size > 0)
        {
            vd.pts = vd.pts - beginTime;
            Data yuv = MediaEncoderFactory::Get()->RGBToYUV(vd);
            vd.Release();
            Data pkt = MediaEncoderFactory::Get()->EncodeVideo(yuv);
            if (pkt.size> 0)
            {
                ////Upstreaming
                if (RTMPModule::Get()->SendFrame(pkt,videoIdx))
                {
                    cout << "@" << flush;
                }
            }
        }
    }
}

bool Controller::Start()
{
    ///1 Filter setting
    VideoCaptureFactory::Get()->AddFilter(Filter::Get());
    ///2 Open camera
    if (camIndex >= 0)
    {
        if (!VideoCaptureFactory::Get()->Init(camIndex))
        {
            err = "open camera failed";
            cout << err << endl;
            return false;
        }
    }

    cout << "Open camera success" << endl;

    ///3 Audio record
    if (!AudioRecorderFactory::Get()->Init())
    {
        err = "Audio init failed";
        cout << err << endl;
        return false;
    }
    cout << "Audio init success" << endl;

    ///11 Start capture data
    AudioRecorderFactory::Get()->Start();
    VideoCaptureFactory::Get()->Start();

    ///Codec
    MediaEncoderFactory::Get()->inWidth = VideoCaptureFactory::Get()->width;
    MediaEncoderFactory::Get()->inHeight = VideoCaptureFactory::Get()->height;
    MediaEncoderFactory::Get()->outWidth = VideoCaptureFactory::Get()->width;
    MediaEncoderFactory::Get()->outHeight = VideoCaptureFactory::Get()->height;
    if (!MediaEncoderFactory::Get()->InitScale())
    {
        return false;
    }

    MediaEncoderFactory::Get()->channels = AudioRecorderFactory::Get()->channels;
    MediaEncoderFactory::Get()->nbSample = AudioRecorderFactory::Get()->nbSamples;
    MediaEncoderFactory::Get()->sampleRate = AudioRecorderFactory::Get()->sampleRate;
    if (!MediaEncoderFactory::Get()->InitResample())
    {
        return false;
    }

    /// Init codec for audio
    if (!MediaEncoderFactory::Get()->InitAudioCodec())
    {
        err = "Init audio codec failed!";
        cout << err << endl;
        return false;
    }
    ///Init codec for video
    if (!MediaEncoderFactory::Get()->InitVideoCodec())
    {
        err = "Init video codec failed!";
        cout << err << endl;
        return false;
    }
    /// Init RTMP module
    if (!RTMPModule::Get()->Init(outUrl.c_str()))
    {
        err = "Init RTMP module failed";
        cout << err << endl;
        return false;
    }

    ///9 Add strean
    videoIdx = RTMPModule::Get()->AddStream(MediaEncoderFactory::Get()->videoCodecContext);
    audioIdx = RTMPModule::Get()->AddStream(MediaEncoderFactory::Get()->audioCodecContext);
    if (videoIdx < 0 || audioIdx < 0)
    {
        err = "Add steam failed!";
        cout << err << endl;
        return false;
    }

    /// Send hea
    if (!RTMPModule::Get()->SendHead())
    {
        err = "Send head failed!";
        cout << err << endl;
        return false;
    }
    AudioRecorderFactory::Get()->Clear();
    VideoCaptureFactory::Get()->Clear();
    DataManager::Start();
    return true;
}

void Controller::Stop()
{
    DataManager::Stop();
    AudioRecorderFactory::Get()->Stop();
    VideoCaptureFactory::Get()->Stop();
    MediaEncoderFactory::Get()->Close();
    RTMPModule::Get()->Close();
    camIndex = -1;
    return;
}

bool Controller::Set(string key, double val)
{
    Filter::Get()->Set(key, val);
    return true;
}

Controller::~Controller()
{

}

Controller::Controller()
{

}
