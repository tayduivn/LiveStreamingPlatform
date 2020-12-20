#include <iostream>
#include "qtaudiorecorder.h"

using namespace std;

QtAudioRecorder::QtAudioRecorder()
{

}

void QtAudioRecorder::run()
{
    // Read a frame data
    int readSize{ nbSamples * channels * sampleByte };
    char *buf = new char[readSize];
    while (!isExit)
    {
        // Get data of a frame
        if (input->bytesReady() < readSize)
        {
            QThread::msleep(1);
            continue;
        }
        int size = 0;
        while (size != readSize)
        {
            int len = io->read(buf + size, readSize - size);
            if (len < 0)
                break;
            size += len;
        }
        if (size != readSize)
        {
            continue;
        }
        // Push data of a frame to list
        Push(Data(buf, readSize, GetCurTime()));
    }
    delete[] buf;
}

bool QtAudioRecorder::Init()
{
    Stop();
    // Config QAudioFormat
    QAudioFormat fmt;
    fmt.setSampleRate(sampleRate);
    fmt.setChannelCount(channels);
    fmt.setSampleSize(sampleByte * 8);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(fmt))
    {
        cout << "Audio format not support!" << endl;
        fmt = info.nearestFormat(fmt);
    }
    input = new QAudioInput(fmt);

    // Begin recording
    io = input->start();
    if (!io)
        return false;
    return true;
}

void QtAudioRecorder::Stop()
{
    DataManager::Stop();
    if (input)
        input->stop();
    if (io)
        io->close();
    input = nullptr;
    io = nullptr;
}
