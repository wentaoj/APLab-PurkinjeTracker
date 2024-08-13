#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>
#include <string>
#include <opencv2/opencv.hpp>
#include "doh_tracker.hpp"
#include "blob.hpp"

using namespace std;
using namespace cv;

void DoHTracker::track(string &src)
{
    Frame generator(src);
    std::vector<TrackerData> pred_info;

    while (generator.hasNext())
    {
        auto [image, frame_num] = generator.next();

        cv::Mat frame = image_proc(image);
        try
        {
            std::vector<Point> centroids = process_frame(frame, frame_num);

            if (centroids.size() < 2)
            {
                throw runtime_error("Not enough blobs found");
            }
            pred_info.push_back(save_results(frame_num, centroids));
        }
        catch (const exception &e)
        {
            pred_info.push_back(TrackerData());
        }
    }
}

std::vector<TrackingResult> DoHTracker::get_tracking_results(string &src)
{
    Frame generator(src);
    std::vector<TrackingResult> results;

    while (generator.hasNext())
    {
        auto [image, frame_num] = generator.next();

        cv::Mat frame = image_proc(image);
        try
        {
            std::vector<Point> centroids = process_frame(frame, frame_num);

            if (centroids.size() < 2)
            {
                throw runtime_error("Not enough blobs found");
            }
            TrackerData data = save_results(frame_num, centroids);
            results.push_back({data.frame_num, data.P1, data.P4, data.dx, data.dy});
        }
        catch (const exception &e)
        {
            results.push_back({frame_num, Point(), Point(), 0, 0});
        }
    }

    return results;
}

std::vector<Point> DoHTracker::process_frame(cv::Mat &frame, int frame_num)
{
    cv::Mat image = frame.clone();
    if (image.channels() == 3)
    {
        cv::cvtColor(image, image, COLOR_BGR2GRAY);
    }

    std::vector<Point> centroids = detect_blobs(image, frame_num);

    if (centroids.size() < 2)
    {
        throw runtime_error("DoH Detection Error: Not enough blobs found at" + to_string(frame_num) + " frame\n");
        return {};
    }
    return centroids;
}

std::vector<Point> DoHTracker::detect_blobs(cv::Mat &image, int frame_num)
{
    DoH doh(max_sigma, min_sigma, num_sigma, threshold, overlap);
    std::vector<Point3f> blobs = doh.get_blobs(image);

    if (blobs.size() < 2)
    {
        return {};
    }

    sort(blobs.begin(), blobs.end(), [](Point3f &a, Point3f &b)
         { return a.z > b.z; });

    std::vector<Point> centroids;
    for (int i = 0; i < blobs.size(); i++)
    {
        float x = blobs[i].x;
        float y = blobs[i].y;
        centroids.push_back(Point(x, y));
    }
    return centroids;
}
