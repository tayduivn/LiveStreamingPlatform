#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <map>

namespace cv
{
class Mat;
}

enum FilterType
{
    BILATERAL
};

class Filter
{
public:
    virtual bool FilterImage(cv::Mat *src, cv::Mat *dest) = 0;

    static Filter *Get(FilterType type = BILATERAL);
    virtual bool Set(std::string key, double value);
    virtual ~Filter();
protected:
    std::map<std::string, double> params;
    Filter();
};

#endif // FILTER_H
