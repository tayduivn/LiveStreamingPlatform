#ifndef VIDEOCAPTUREABSTRACT_H
#define VIDEOCAPTUREABSTRACT_H

#include "data/datamanager.h"
#include "beautify/filter.h"

class VideoCaptureAbstract : public DataManager
{
public:
    virtual bool Init(int camIndex = 0) = 0;
    virtual void Stop() = 0;

    void AddFilter(Filter *filter){
        mutex.lock();
        filters.push_back(filter);
        mutex.unlock();
    }
    int width = 0;
    int height = 0;
    int fps = 0;
protected:
    std::mutex mutex;
    std::vector<Filter*> filters;
};

#endif // VIDEOCAPTUREABSTRACT_H
