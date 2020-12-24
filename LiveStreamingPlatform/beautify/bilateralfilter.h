#ifndef BILATERAFILTER_H
#define BILATERAFILTER_H

#include "filter.h"

class BilateralFilter : public Filter
{
public:
    BilateralFilter();
    bool FilterImage(cv::Mat *src, cv::Mat *dest);
    virtual ~BilateralFilter();
};

#endif // BILATERAFILTER_H
