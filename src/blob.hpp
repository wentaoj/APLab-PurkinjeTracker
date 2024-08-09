#pragma once

#include "vector"
#include "utility"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

struct Point3
{
    float y, x, z;
    Point3(float y, float x, float z) : x(x), y(y), z(z) {}
};

std::vector<float> logspace(float start, float stop, int num);
vector<int> argsort(vector<float> &inten);

class DoH
{
public:
    DoH(int max_sigma, int min_sigma, int num_sigma, float threshold, float overlap = 0.2) : max_sigma(max_sigma), min_sigma(min_sigma), num_sigma(num_sigma), threshold(threshold), overlap(overlap) {}

    // Finds blobs in the given grayscale image.
    vector<Point3f> get_blobs(Mat &in_image);

private:
    int max_sigma;
    int min_sigma;
    int num_sigma;
    float threshold;
    float overlap;

    // Eliminates blobs with area overlap.
    vector<Point3f> get_prune(vector<Point3f> &blob, float overlap);
    float integral_img(Mat &image, int r, int c, int rl, int cl);

    // Return the peak coordinates.
    vector<Point3f> get_peak(vector<Mat> &image, vector<Mat> &mask, int num_peaks);

    // Return the mask containing all peak candidates above thresholds.
    vector<Mat> get_mask(vector<Mat> &image, vector<Mat> &footprint, float threshold);

    // Find peaks in an image as coordinate list.
    vector<Point3f> local_max(vector<Mat> &image, float threshold = 0.01, int num_peaks = 25);
    Mat hessian_mat(Mat &image, float sigma);
};
