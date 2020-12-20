#include "data.h"
#include <stdlib.h>
#include <string.h>

extern "C"
{
#include <libavutil/time.h>
}

Data::Data()
{
}

Data::Data(char * data, int size, long long pts)
{
    this->data = new char[size];
    memcpy(this->data, data, size);
    this->size = size;
    this->pts = pts;
}


Data::~Data()
{
}

void Data::Release()
{
    if (data)
        delete data;
    data = 0;
    size = 0;
}

long long GetCurTime()
{
    return av_gettime();
}
