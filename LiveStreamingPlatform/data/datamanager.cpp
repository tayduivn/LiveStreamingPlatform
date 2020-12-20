#include "datamanager.h"

DataManager::DataManager()
{
}


DataManager::~DataManager()
{
}

void DataManager::Push(Data d)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (datas.size() > maxList)
    {
        datas.front().Release();
        datas.pop_front();
    }
    datas.push_back(d);
}

Data DataManager::Pop()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (datas.empty())
    {
        return Data();
    }
    Data d = datas.front();
    datas.pop_front();
    return d;
}

void DataManager::Start()
{
    isExit = false;
    QThread::start();
}

void DataManager::Stop()
{
    isExit = true;
    wait();
}
