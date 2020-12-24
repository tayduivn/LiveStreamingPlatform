#include "filter.h"
#include "bilateralfilter.h"
#include <iostream>

using namespace std;

Filter *Filter::Get(FilterType type)
{
    static BilateralFilter bilateralFilter;
    switch (type)
    {
    case BILATERAL:
        return &bilateralFilter;
        break;
    default:
        break;
    }
    return nullptr;
}

bool Filter::Set(string key, double value)
{
    if (params.find(key) == params.end())
    {
        cout << "para " << key << " is not support!" << endl;
        return false;
    }
    params[key] = value;
    return true;
}

Filter::~Filter()
{

}

Filter::Filter()
{

}
