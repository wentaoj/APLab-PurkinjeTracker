#ifndef BASE_TRACKER_HPP
#define BASE_TRACKER_HPP

#include "vector"
#include "utility"
#include "string"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

struct TrackerData
{
    int frame_num;
    Point P1;
    Point P4;
    float dx;
    float dy;
};

class BaseTracker
{
public:
    virtual void track(string &src) = 0;
    ~BaseTracker() {}
};

class IntermediaryTracker : public BaseTracker
{
public:
    // Generator function for reading frames from video.
    vector<pair<Mat, int>> read_frames(string &src);
    Mat image_proc(Mat &frame);

    // Calculate dx, dy and save the centroids of two blobs.
    TrackerData save_results(int frame_num, vector<Point> &centroids);

    // void save_preds(vector<TrackerData> &data_vec, string &src_video);
};

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

#endif