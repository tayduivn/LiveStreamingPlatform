#ifndef QTAUDIORECORDER_H
#define QTAUDIORECORDER_H

#include <QAudioInput>
#include "audiorecorderabstract.h"
#include "data/datamanager.h"

class QtAudioRecorder : public AudioRecorderAbstract
{
public:
    QtAudioRecorder();
    void run() override;

    bool Init() override;
    void Stop() override;

private:
    QAudioInput *input = nullptr;
    QIODevice *io = nullptr;
};

#endif // QTAUDIORECORDER_H
