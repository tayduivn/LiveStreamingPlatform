#include "audiorecorderfactory.h"

AudioRecorderAbstract *AudioRecorderFactory::Get(RECORDMETHOD method, unsigned index)
{
    static QtAudioRecorder instance[305];
    return &instance[index];
}

AudioRecorderFactory::AudioRecorderFactory()
{

}
