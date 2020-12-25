#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QThread>
#include <list>
#include <mutex>
#include "Data.h"

class DataManager : public QThread
{
public:
    DataManager();
    virtual ~DataManager();

    // max size of list
    int maxList = 100;

    // Push data to the last of list
    virtual void Push(Data d);

    // Get front data from the list
    virtual Data Pop();

    // Start the thread
    virtual bool Start();

    // Stop the thread
    virtual void Stop();

    // Clear the data in queue
    virtual void Clear();

protected:
    // List for storing data, FIFO
    std::list<Data> datas;

    // Number of data
    int dataCount = 0;

    // Mutex datas;
    std::mutex mutex;

    // Flag for stop the thread
    bool isExit = false;
};

#endif // DATAMANAGER_H
