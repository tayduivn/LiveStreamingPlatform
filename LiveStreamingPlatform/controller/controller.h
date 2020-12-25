#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QThread>
#include <string>
#include "data/datamanager.h"

class Controller : public DataManager
{
public:
    std::string outUrl;
    int camIndex = 0;
    std::string err = "";

    static Controller *Get();

    void run();
    virtual bool Start();
    virtual void Stop();

    // Parameter setting
    virtual bool Set(std::string key, double val);

    virtual ~Controller();
protected:
    int videoIdx = 0; // video stream index
    int audioIdx = 1; // audio stream index
    Controller();
private:
    static Controller *mInstance;
};

#endif // CONTROLLER_H
