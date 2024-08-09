#pragma once

#include "vector"
#include "utility"
#include "string"
#include "opencv2/opencv.hpp"
#include "blob.hpp"
#include "base_tracker.hpp"

using namespace std;
using namespace cv;

struct TrackingResult
{
    int frame_num;
    Point P1;
    Point P4;
    float dx;
    float dy;
};

// Implements blob tracking using Difference of Hessian method.
class DoHTracker : public IntermediaryTracker
{
public:
    DoHTracker(int max_sigma, int min_sigma, int num_sigma, float threshold, float overlap) : max_sigma(max_sigma), min_sigma(min_sigma), num_sigma(num_sigma), threshold(threshold), overlap(overlap) {}

    // Tracks blobs in the video specified by src.
    void track(string &src) override;

    vector<TrackingResult> get_tracking_results(string &src);

    // Processes a single frame to find blob centroids.
    vector<Point> process_frame(Mat &frame, int frame_num);

private:
    int max_sigma;
    int min_sigma;
    int num_sigma;
    float threshold;
    float overlap;

    // Detects blobs in a frame using the DoH method.
    vector<Point> detect_blobs(Mat &frame, int frame_num);
};
