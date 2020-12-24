#include "bilateralfilter.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

BilateralFilter::BilateralFilter()
{
    params.insert(make_pair("d", 5));
    params.insert(make_pair("sigmaColor", 10));
    params.insert(make_pair("sigmaSpace", 2.5));
}

bool BilateralFilter::FilterImage(cv::Mat *src, cv::Mat *dest)
{
    double d = params["d"];
    double sigmaColor = params["sigmaColor"];
    double sigmaSpace = params["sigmaSpace"];
    bilateralFilter(*src, *dest, d, sigmaColor, sigmaSpace);
    return true;
}

BilateralFilter::~BilateralFilter()
{

}
