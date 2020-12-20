#ifndef AUDIORECORDERFACTORY_H
#define AUDIORECORDERFACTORY_H

#include "qtaudiorecorder.h"

enum RECORDMETHOD
{
    QT_AUDIO
};

class AudioRecorderFactory
{
public:
    static AudioRecorderAbstract *Get(RECORDMETHOD method = QT_AUDIO, unsigned index = 0);

protected:
    AudioRecorderFactory();
};

#endif // AUDIORECORDERFACTORY_H
