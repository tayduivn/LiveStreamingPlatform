#ifndef AUDIORECORDERABSTRACT_H
#define AUDIORECORDERABSTRACT_H

#include "data/datamanager.h"

class AudioRecorderAbstract : public DataManager
{
public:
    // Begin recording
    virtual bool Init() = 0;

    // Stop recording
    virtual void Stop() = 0;

    // Number of channel
    int channels = 2;

    // Sampling rating
    int sampleRate = 44100;

    int sampleByte = 2;

    // Number of sampling
    int nbSamples = 1024;

};

#endif // AUDIORECORDERABSTRACT_H
