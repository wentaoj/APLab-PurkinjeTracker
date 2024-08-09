#pragma once

#include "vector"
#include "utility"
#include "string"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;
using namespace chrono;

template <typename Func, typename... Args>
auto timingdecorator(Func func, Args... args) -> typename enable_if<is_void<decltype(func(args...))>::value>::type
{
    auto start = high_resolution_clock::now();
    func(args...);
    auto end = high_resolution_clock::now();
    duration<double, milli> elapsed = end - start;
    cout << "Elapsed time: " << elapsed.count() << " ms\n";
}

class Frame
{
private:
    VideoCapture capture;
    int frame = 0;
    Mat curr;

public:
    Frame(string &vid_src)
    {
        capture.open(vid_src);
        if (!capture.isOpened())
        {
            throw runtime_error("Could not open video file: " + vid_src);
        }
    }

    bool hasNext()
    {
        if (capture.read(curr))
        {
            return true;
        }
        return false;
    }

    pair<Mat, int> next()
    {
        if (curr.empty())
        {
            throw out_of_range("No more frames.");
        }
        pair<Mat, int> result = {curr.clone(), frame++};
        curr.release();
        return result;
    }

    ~Frame()
    {
        capture.release();
    }
};
