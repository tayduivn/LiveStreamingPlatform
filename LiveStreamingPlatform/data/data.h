#ifndef DATA_H
#define DATA_H

class Data
{
public:
    Data();
    Data(char *data, int size, long long pts =0);
    virtual ~Data();

    void Release();
    char *data = 0;
    int size = 0;
    long long pts = 0;
};

long long GetCurTime();
#endif // DATA_H
